// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <baba-is-auto/Games/Game.hpp>

namespace baba_is_auto
{

Game::Game(std::string_view filename)
{
    m_map.Load(filename);
    m_ruleManager.ParseRules(m_map);
    m_playState = PlayState::PLAYING;

    std::random_device rnd;
    mt.seed(rnd());
}

void Game::Reset()
{
    m_map.Reset();
    m_ruleManager.ParseRules(m_map);
    m_playState = PlayState::PLAYING;
}

Map& Game::GetMap()
{
    return m_map;
}

const Map& Game::GetMap() const
{
    return m_map;
}

RuleManager& Game::GetRuleManager()
{
    return m_ruleManager;
}

PlayState Game::GetPlayState() const
{
    return m_playState;
}

int Game::RandInt(int min, int max)
{
    std::uniform_int_distribution<> rand(min, max);
    return rand(mt);
}

Direction Game::SetRandomDirectionToObject(Object& obj){
    Direction dirs[] = {Direction::LEFT,
			Direction::RIGHT,
			Direction::UP,
			Direction::DOWN};
    Direction dir = dirs[RandInt(0, 3)];
    dir = Direction::LEFT;
    obj.SetDirection(dir);
    return dir;

}

void Game::MovePlayer(Direction dir)
{

    // This function is directly called in simulation.

    /*
      Notes (letra418):
      In the original baba-is-you, parsing of rules occurs many times in a step and the flow is as follows:
      1. normal movements (YOU, move, fear, shift, etc.) -> rule parsing
      2. object changes (is, up/down/right/left, etc.)-> rule parsing
      3. special movements (tele, fall, etc.) -> rule parsing
      4. object vanishments (sink, weak, hot/melt, etc.) -> rule parsing

      e.g., when a flag is on a belt and making BELT IS SHIFT, the flag is shifted in the next step. However, when making BELT IS TELE, the flag teleports in the current step.

      <ref>
      - https://w.atwiki.jp/babais/pages/42.html#id_12b90dfb
    */

    // ===========================
    // 1-1. Normal movements
    ProcessYOU(dir);
    ProcessMOVE();
    ProcessSHIFT();
    // m_ruleManager.ParseRules(m_map);
    // ===========================
    // 2. Objects changes
    ProcessIS();
    //m_ruleManager.ParseRules(m_map);

    // ===========================
    // 3. Special Movements
    // ProcessTele();
    // m_ruleManager.ParseRules(m_map);

    // ===========================
    // 4. Objects vanishments
    ProcessSINK();
    ProcessHOTAndMELT();
    ProcessDEFEAT();
    m_ruleManager.ParseRules(m_map);

    // ===========================
    // 5. Check Won/List
    CheckPlayState();
}

Direction GetReverseDirection(Direction dir){
    if (dir == Direction::LEFT){ return Direction::RIGHT; }
    if (dir == Direction::RIGHT){ return Direction::LEFT; }
    if (dir == Direction::UP){ return Direction::DOWN; }
    if (dir == Direction::DOWN){ return Direction::UP; }
    return Direction::NONE;
}

std::tuple<int, int> GetPositionAfterMove(std::size_t x, std::size_t y, Direction dir)
{
    int _x = static_cast<int>(x);
    int _y = static_cast<int>(y);

    int dx = 0, dy = 0;
    if (dir == Direction::UP)
    {
        dy = -1;
    }
    else if (dir == Direction::DOWN)
    {
        dy = 1;
    }
    else if (dir == Direction::LEFT)
    {
        dx = -1;
    }
    else if (dir == Direction::RIGHT)
    {
        dx = 1;
    }

    _x += dx;
    _y += dy;
    return std::forward_as_tuple(_x, _y);
}

std::tuple<int, int> GetPositionOfPulledObject(std::size_t x, std::size_t y, 
					       Direction dir)
{
    int _x = static_cast<int>(x);
    int _y = static_cast<int>(y);

    int dx = 0, dy = 0;
    if (dir == Direction::UP)
    {
        dy = +1;
    }
    else if (dir == Direction::DOWN)
    {
        dy = -1;
    }
    else if (dir == Direction::LEFT)
    {
        dx = 1;
    }
    else if (dir == Direction::RIGHT)
    {
        dx = -1;
    }

    _x += dx;
    _y += dy;
    return std::forward_as_tuple(_x, _y);
}


bool Game::CanMove(std::size_t x, std::size_t y, Direction dir, 
		   const Object& srcObject) const
{
    const auto width = static_cast<int>(m_map.GetWidth());
    const auto height = static_cast<int>(m_map.GetHeight());

    int _x;
    int _y;
    std::tie(_x, _y) = GetPositionAfterMove(x, y, dir);

    // Check boundary
    if (_x < 0 || _x >= width || _y < 0 || _y >= height){
        return false;
    }

    const ObjectContainer& dstObjects = m_map.GetObjects(_x, _y);

    // return false if one of objects at the target postion cannot move to the direction
    for (auto & obj: dstObjects){
	/*
	  Notes (letra418):
	  - TODO: implement SHUT, OPEN, PULL, WEAK, SWAP, FLOAT
	*/

	if (m_ruleManager.HasType(obj, m_map, ObjectType::PUSH) &&
	    !CanMove(_x, _y, dir, obj)){
	    return false;
	}
	else if (m_ruleManager.HasType(obj, m_map, ObjectType::STOP)){
	    return false;
	}
    }
    return true;
}

void Game::ProcessYOU(Direction dir)
{
    if (dir == Direction::NONE) { return; }
    int _x;
    int _y;
    auto obj_ids = FindObjectIdsAndPositionsByType(ObjectType::YOU);


    /*
      MEMO: 
      - 複数のYOUが並んでいるときにあるYOUが先に動いた結果，他のYOUをPUSHしてしまう
      - 複数のYOUが重なっている時に一番上のYOUが先に動いた結果，次のYOUが動く時に進路をブロックしてバラけてしまう
      - 変化を記録するような形にする？
        * youの移動とyouの移動の効果を別にする必要もある

      - ルール適用の優先度でループを分けた方が良い？

      - 同時解決の処理順についてはどうなる？
        * 基本的に同じルールによる処理は同時
	  * MOVEによるPUSHでTEXTを重ねられるし、自身がPushでも重なることが出来る
	* 一斉に判定・MOVEして、一斉にPUSHすれば解決する？

	* ただし同じルールによって引き起こされるPUSHには順序がある場合も
	  * 同じものを２方向からPUSHした際に、ベクトル演算のようにはならない
	  * ただし２方向から同じマスにPUSHされた時は重なるので同時に判定されている
	* 今動いている途中かどうかを示すフラグが必要？
	  * moveした各objectを始点として、そこからPUSH可能なものが続く限りどちら方向に動くかフラグを立て、それらを順に解決？２方向からの場合どちらかが優先。同じルールで2回PUSHされることはないはず。

      - YOUやMOVEで動く本人もフラグで管理して解決を後回しにする必要がある？
        * 縦に並んだYOUや、MOVEで同じマスに入るときなど、移動後のobjectにpush flagを立ててしまう可能性あり。
	* ただ、そのフラグの付与はPUSHの処理よりも優先される。例えばMOVEで動くはずのobjが先に他のobjによりフラグを付与されてしまって、本来のMOVEを行えない事がありうる
     */
    /*
      Notes (letra418):
    */

    for (auto& [obj_id, x, y] : obj_ids){
 	Object& srcObject = m_map.GetObject(obj_id, x, y);
	srcObject.SetDirection(dir); // Notes: Segmentation Fault
	if (!CanMove(x, y, dir, srcObject)) continue;
	srcObject.SetMoveFlag(dir);
    }

    // Setting a move direction to pushed objects needs to be done after setting it to moving objects.
    // Otherwise a pushed object can set a different direction to moving objects.
    for (auto& [obj_id, x, y] : obj_ids){
 	Object& srcObject = m_map.GetObject(obj_id, x, y);
	if (!CanMove(x, y, dir, srcObject)) continue;
 	std::tie(_x, _y) = GetPositionAfterMove(x, y, dir);
	SetPushedDirToObjects(_x, _y, dir);
    }
    ResolveAllMoveFlags();
}

void Game::ProcessSHIFT()
{
    Direction dir;
    auto obj_ids = FindObjectIdsAndPositionsByType(ObjectType::SHIFT);

    for (auto& [obj_id, x, y] : obj_ids){
    	Object& obj = m_map.GetObject(obj_id, x, y);
    	dir = obj.GetDirection();
    	if (dir == Direction::NONE){
    	    dir = SetRandomDirectionToObject(obj);
    	}
    	for (auto& tgtObj: m_map.GetObjects(x, y)){
    	    if (tgtObj.GetId() == obj.GetId()) continue;
    	    tgtObj.SetDirection(dir);
    	    tgtObj.SetMoveFlag(dir);
    	}
    }

    int _x;
    int _y;
    for (auto& [obj_id, x, y] : obj_ids){
    	Object& obj = m_map.GetObject(obj_id, x, y);
	// If there are no objects on the SHIFT object, it pushes nothing.
	bool something_on_shift = false;
    	dir = obj.GetDirection();
	for (auto& tgtObj: m_map.GetObjects(x, y)){
	    if (tgtObj.GetId() != obj.GetId()){
		something_on_shift = true;
	    }
	}
	if (!something_on_shift) continue;

    	std::tie(_x, _y) = GetPositionAfterMove(x, y, dir);
    	SetPushedDirToObjects(_x, _y, dir);
    }

    ResolveAllMoveFlags();
}


void Game::ProcessMOVE()
{
    int _x;
    int _y;
    Direction dir;
    Direction revdir;
    auto obj_ids = FindObjectIdsAndPositionsByType(ObjectType::MOVE);

    for (auto& [obj_id, x, y] : obj_ids){
    	Object& obj = m_map.GetObject(obj_id, x, y);
	dir = obj.GetDirection();
	if (dir == Direction::NONE){
	    dir = SetRandomDirectionToObject(obj);
	    dir = obj.GetDirection();
	}
	revdir = GetReverseDirection(dir);

    	if (CanMove(x, y, dir, obj)){
	    std::tie(_x, _y) = GetPositionAfterMove(x, y, dir);
	    obj.SetMoveFlag(dir);
	} else if (CanMove(x, y, revdir, obj)){
	    obj.SetDirection(revdir);
	    obj.SetMoveFlag(revdir);
	} else {
	    obj.SetDirection(revdir);
	}
    }

    for (auto& [obj_id, x, y] : obj_ids){
    	Object& obj = m_map.GetObject(obj_id, x, y);
	dir = obj.GetDirection();
	revdir = GetReverseDirection(dir);

    	if (CanMove(x, y, dir, obj)){
	    std::tie(_x, _y) = GetPositionAfterMove(x, y, dir);
	    SetPushedDirToObjects(_x, _y, dir);
	} else {
	    std::tie(_x, _y) = GetPositionAfterMove(x, y, revdir);
	    SetPushedDirToObjects(_x, _y, revdir);
	}
    }

    ResolveAllMoveFlags();
}

void Game::ProcessIS()
{
    /*
      memo:
      - (todo) 変化する際に前のobjectを残さないとA is B, A is Cに対応できない

     */
    auto is_noun_rules = m_ruleManager.GetRules(ObjectType::IS);

    for (auto& rule: is_noun_rules){
	ObjectType subjType = rule.GetSubject();
	ObjectType predType = rule.GetPredicate();
	if (IsPropertyType(predType) || rule.GetOperator() != ObjectType::IS) continue;

	subjType = (subjType != ObjectType::TEXT) ? ConvertTextToIcon(subjType) : subjType;
	predType = (predType != ObjectType::TEXT) ? ConvertTextToIcon(predType) : ConvertIconToText(subjType);

	auto obj_ids = FindObjectIdsAndPositionsByType(subjType);
	for (auto& [obj_id, x, y] : obj_ids){
	    Object& obj = m_map.GetObject(obj_id, x, y);
	    obj.SetChangeFlag(predType);
	}
    }
    ResolveAllChangeFlags();

    // auto itr = std::remove_if(m_objects.begin(), m_objects.end(), 
    // 			      [&](Object x){
    // 				  return x.GetType() == type; 
    // 			      }); 
    // // auto obj_ids = 
    // for (std::vector<int>:iterator it = std::find_if(v.begin(), v.end(), IsOdd);
    // 	 it != v.end();
    // 	 it = std::find_if(++it, v.end(), IsOdd))
    // 	{
    // 	    // ...
    // 	}

    // auto rules = m_ruleManager.GetRules([&](Rule r){
    // 					    if (IsNounType(r.GetSubject()) && 
    // 						r.GetOperator() == ObjectType::IS && 
    // 						IsNounType(r.GetPredicate()))
    // 						{
    // 						    return true;
    // 						}
    // 					    else
    // 						{
    // 						    return false;
    // 						}
    // 					});
    // for (auto& rule : rules){
    // 	std::cout << rule.GetSubject() << " "
    // 		  << rule.GetOperator() << " "
    // 		  << rule.GetObject() << " "
    // 		  << std::endl;
    // }

    return;
}

bool Game::ProcessSINK()
{
    bool happened = false;
    auto obj_ids = FindObjectIdsAndPositionsByType(ObjectType::SINK);

    // All objects on SINK object and itself are removed.
    for (auto& [_, x, y] : obj_ids){
	auto& objs = m_map.GetObjects(x, y);
	if (objs.size() > 1){
	    for (auto& obj : objs){
		obj.SetRemoveFlag(true);
		happened = true;
	    }
	}
    }
    ResolveAllRemoveFlags();
    return happened;
}

bool Game::ProcessHOTAndMELT()
{
    bool happened = false;
    auto obj_ids = FindObjectIdsAndPositionsByType(ObjectType::MELT);
    for (auto& [melt_id, x, y] : obj_ids){
	auto& meltObj = m_map.GetObject(melt_id, x, y);

	for (auto& obj : m_map.GetObjects(x, y)){
	    if (m_ruleManager.HasType(obj, m_map, ObjectType::HOT)){
		meltObj.SetRemoveFlag(true);
		happened = true;
	    }
	}
    }
    ResolveAllRemoveFlags();
    return happened;
}

bool Game::ProcessDEFEAT()
{
    bool happened = false;
    auto obj_ids = FindObjectIdsAndPositionsByType(ObjectType::YOU);
    for (auto& [you_id, x, y] : obj_ids){
	auto& youObj = m_map.GetObject(you_id, x, y);

	for (auto& obj : m_map.GetObjects(x, y)){
	    if (m_ruleManager.HasType(obj, m_map, ObjectType::DEFEAT)){
		youObj.SetRemoveFlag(true);
		happened = true;
	    }
	}
    }
    ResolveAllRemoveFlags();
    return happened;
}








void Game::CheckPlayState() // todo
{
    /*
      Notes (letra418):
      Strictly, existing no YOU rules or YOU objects does not mean lose.
    */

    const auto youRules = m_ruleManager.GetRules(ObjectType::YOU);
    if (youRules.empty())
    {
        m_playState = PlayState::LOST;
        return;
    }

    auto obj_ids = FindObjectIdsAndPositionsByType(ObjectType::YOU);
    if (obj_ids.empty())
    {
        m_playState = PlayState::LOST;
        return;
    }
    for (auto& [_, x, y] : obj_ids){
	auto& objs = m_map.GetObjects(x, y);
	for (auto & obj: objs){
	    if (m_ruleManager.HasType(obj, m_map, ObjectType::WIN)){
		m_playState = PlayState::WON;
	    }
	}
    }
}

// Instead of returning references to objects, this function returns tuples of (ObjectId, X, Y). This is due to subsequent lost of references caused by memory reallocation of std::vector when an object is moved from a square to another square..
std::vector<PositionalObject> Game::FindObjectIdsAndPositionsByType(ObjectType objtype){
    std::vector<PositionalObject> res;

    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    for (std::size_t y = 0; y < height; ++y){
        for (std::size_t x = 0; x < width; ++x){
	    ObjectContainer& objs = m_map.GetObjects(x, y);
	    for (auto itr = objs.begin(), e = objs.end(); itr != e; ++itr){ 
		if (IsIconType(objtype) && (itr->GetType() == objtype)){
		    std::tuple t = std::make_tuple(itr->GetId(), x, y);
		    res.emplace_back(t);
		} else if (IsPropertyType(objtype) && m_ruleManager.HasType(*itr, m_map, objtype)){
		    std::tuple t = std::make_tuple(itr->GetId(), x, y);
		    // std::tuple t = std::tie(itr->GetId(), x, y);
		    res.emplace_back(t);
		}
	    }
        }
    }

    return res;
}

void Game::SetPushedDirToObjects(std::size_t x, std::size_t y, Direction dir){
    // Recursively add pushed_flag to PUSH objects on squares.
    // This function stops when no PUSH objects exist on the next square.

    ObjectContainer& objs = m_map.GetObjects(x, y);
    bool continue_pushing = false;

    for (auto itr = objs.begin(), e = objs.end(); itr != e; ++itr){
	if (m_ruleManager.HasType(*itr, m_map, ObjectType::PUSH)){
	    // Skipped if an object was already pushed from another direction (e.g., MOVE objects can push an object from two directions).
	    if (itr->GetMoveFlag() == Direction::NONE){
		if (CanMove(x, y, dir, *itr)){
		    itr->SetMoveFlag(dir);
		    itr->SetDirection(dir);
		}
		itr->SetMoveFlag(dir);
		itr->SetDirection(dir);
		continue_pushing = true;
	    }
	}
    }
    // Continue pushing if at least one of objects is pushed to the direction.
    if (continue_pushing){
	int _x;
	int _y;
	std::tie(_x, _y) = GetPositionAfterMove(x, y, dir);
	SetPushedDirToObjects(_x, _y, dir);
    }
    return;
}


void Game::ResolveAllChangeFlags(){
    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();
    ObjectType change_to;
    std::vector<std::tuple<ObjectId, size_t, size_t, ObjectType>> objsChangeSchedule;
    std::tuple<ObjectId, size_t, size_t, ObjectType> s;
    for (std::size_t y = 0; y < height; ++y){
        for (std::size_t x = 0; x < width; ++x){
	    ObjectContainer& objs = m_map.GetObjects(x, y);
	    for (auto itr = objs.begin(), e = objs.end(); itr != e; ++itr){
		change_to = itr->GetChangeFlag();
		if (change_to != itr->GetType()){
		    s = std::make_tuple(itr->GetId(), x, y, change_to);
		    objsChangeSchedule.emplace_back(s);
		}
	    }
	}
    }
    for (auto& [obj_id, x, y, change_to] : objsChangeSchedule){
	Object& obj = m_map.GetObject(obj_id, x, y);
	obj.SetType(change_to);
	obj.SetChangeFlag(change_to);
    }
}

void Game::ResolveAllRemoveFlags(){
    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    std::vector<std::tuple<ObjectId, size_t, size_t>> objsRemoveSchedule;
    std::tuple<ObjectId, size_t, size_t> s;

    for (std::size_t y = 0; y < height; ++y){
        for (std::size_t x = 0; x < width; ++x){
	    ObjectContainer& objs = m_map.GetObjects(x, y);
	    for (auto itr = objs.begin(), e = objs.end(); itr != e; ++itr){
		if (itr->GetRemoveFlag()){
		    s = std::make_tuple(itr->GetId(), x, y);
		    objsRemoveSchedule.emplace_back(s);
		}
	    }
	}
    }
    for (auto& [obj_id, x, y] : objsRemoveSchedule){
	Object& obj = m_map.GetObject(obj_id, x, y);
	m_map.RemoveObject(x, y, obj);
    }
}


void Game::ResolveAllMoveFlags(){
    int _x; 
    int _y;
    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    std::vector<std::tuple<ObjectId, size_t, size_t, Direction>> objsMoveSchedule;
    std::tuple<ObjectId, size_t, size_t, Direction> s;

    for (std::size_t y = 0; y < height; ++y){
        for (std::size_t x = 0; x < width; ++x){
	    ObjectContainer& objs = m_map.GetObjects(x, y);

	    for (auto itr = objs.begin(), e = objs.end(); itr != e; ++itr){
		Direction dir = itr->GetMoveFlag();
		if (dir != Direction::NONE){
 		    // std::cout << "ResolveAllMoveFlags" << std::endl;
		    // Std::cout << static_cast<int>(itr->GetType()) << " "
		    // 	      << static_cast<int>(itr->GetId()) << " "
		    // 	      << x << " "
		    // 	      << y << " "
		    // 	      << std::endl;

		    itr->SetMoveFlag(Direction::NONE);
		    s = std::make_tuple(itr->GetId(), x, y, dir);
		    objsMoveSchedule.emplace_back(s);
		}
	    }
        }
    }
    for (auto& [obj_id, x, y, dir] : objsMoveSchedule){
	Object& obj = m_map.GetObject(obj_id, x, y);
	std::tie(_x, _y) = GetPositionAfterMove(x, y, dir);
	if (CanMove(x, y, dir, obj)){
	    m_map.AddObject(_x, _y, obj);
	    m_map.RemoveObject(x, y, obj);
	}
    }

}

} // namespace baba_is_auto

