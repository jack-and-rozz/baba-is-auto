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
    ParseRules();
    m_playState = PlayState::PLAYING;
}

void Game::Reset()
{
    m_map.Reset();

    ParseRules();

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


void Game::MovePlayer(Direction dir)
{

    /*
      Notes (letra418):
      Strictly speaking, the current order of each process is incomplete.
      In the original game, parsing of rules occurs many times and the flow is as follows:
      1. normal movements (YOU, move, fear, shift, etc.) -> rule parsing
      2. object changes (is, direction changes, etc.)-> rule parsing
      3. special movements (tele, fall, etc.) -> rule parsing
      4. object vanishments (sink, weak, hot/melt, etc.) -> rule parsing

      <ref>
      - https://w.atwiki.jp/babais/pages/42.html#id_12b90dfb
    */

    // 1-1. Movement by YOU
    if (dir != Direction::NONE){
	ProcessMoveByYou(dir);
    }

    // 2. Parsing Rules
    ParseRules();

    // 3. Update Objects
    UpdateObjects();

    // 4. Check Won/List
    CheckPlayState();
}

void Game::ParseRules()
{
    m_ruleManager.ClearRules();

    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            m_map.At(x, y).isRule = false;
        }
    }

    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            ParseRule(x, y, RuleDirection::HORIZONTAL);
            ParseRule(x, y, RuleDirection::VERTICAL);
        }
    }

    // m_playerIcons = m_ruleManager.GetSubjectsByPredicate(ObjectType::YOU);
}

void Game::ParseRule(std::size_t x, std::size_t y, RuleDirection direction)
{
    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    if (direction == RuleDirection::HORIZONTAL)
    {
        if (x + 2 >= width)
        {
            return;
        }
	// Currently an edge case is ignored where two texts are overlapping.

        if (m_map.At(x, y).HasNounType() && m_map.At(x + 1, y).HasVerbType() &&
            (m_map.At(x + 2, y).HasNounType() ||
             m_map.At(x + 2, y).HasPropertyType()))
	{
	    auto type1 = m_map.At(x, y).GetTextObjects()[0].GetType();
	    auto type2 = m_map.At(x + 1, y).GetTextObjects()[0].GetType();
	    auto type3 = m_map.At(x + 2, y).GetTextObjects()[0].GetType();

	    Rule newRule = Rule(type1, type2, type3);
	    m_ruleManager.AddRule(newRule);

	    m_map.At(x, y).isRule = true;
	    m_map.At(x + 1, y).isRule = true;
	    m_map.At(x + 2, y).isRule = true;
	}
    }
    else if (direction == RuleDirection::VERTICAL)
    {
        if (y + 2 >= height)
        {
            return;
        }

        if (m_map.At(x, y).HasNounType() && m_map.At(x, y + 1).HasVerbType() &&
            (m_map.At(x, y + 2).HasNounType() ||
             m_map.At(x, y + 2).HasPropertyType()))
	{
	    auto type1 = m_map.At(x, y).GetTextObjects()[0].GetType();
	    auto type2 = m_map.At(x, y + 1).GetTextObjects()[0].GetType();
	    auto type3 = m_map.At(x, y + 2).GetTextObjects()[0].GetType();

	    Rule newRule = Rule(type1, type2, type3);
	    m_ruleManager.AddRule(newRule);

            m_map.At(x, y).isRule = true;
            m_map.At(x, y + 1).isRule = true;
            m_map.At(x, y + 2).isRule = true;
	}
    }
}

std::tuple<int, int> GetPositionsAfterMove(std::size_t x, std::size_t y, 
					   Direction dir)
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

std::tuple<int, int> GetPositionsOfPulledObject(std::size_t x, std::size_t y, 
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

    int _x; // after move
    int _y; // after move
    std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);

    // Check boundary
    if (_x < 0 || _x >= width || _y < 0 || _y >= height){
        return false;
    }

    auto dstSquare = m_map.At(_x, _y);
    const ObjectContainer dstObjects = dstSquare.GetObjects();

    // return false if one of objects at the target postion cannot move to the direction
    for (auto & obj: dstObjects){
	/*
	  Notes (letra418):
	  - TODO: implement SHUT, OPEN, PULL, WEAK, SWAP, FLOAT
	*/

	if (m_ruleManager.HasType(obj, dstSquare, m_map, ObjectType::PUSH) && 
	    !CanMove(_x, _y, dir, obj)){
	    return false;
	}
	else if (m_ruleManager.HasType(obj, dstSquare, m_map, ObjectType::STOP)){
	    return false;
	}
    }
    return true;
}

void Game::ProcessMoveByYou(Direction dir)
{
    std::cout << "<ProcessMoveByYou: startt>" << std::endl;

    auto you_ids = FindObjectIdsAndPositionsByProperty(ObjectType::YOU);

    int _x;
    int _y;

    /*
      TODO (bug): 
      - 複数のYOUが並んでいるときにあるYOUが先に動いた結果，他のYOUをPUSHしてしまう
      - 複数のYOUが重なっている時に一番上のYOUが先に動いた結果，次のYOUが動く時に進路をブロックしてバラけてしまう
      - 変化を記録するような形にする？
        * youの移動とyouの移動の効果を別にする必要もある

      - ルール適用の優先度でループを分けた方が良い？

      - 同時解決の処理順についてはどうなる？
        * 基本的に同じルールによる処理は同時
	  * MOVEによるPUSHでTEXTを重ねられるし、自身がPushでも重なることが出来る
	* 一斉にMOVEして、一斉にPUSHすれば解決する？

	* ただし同じルールによって引き起こされるPUSHには順序がある場合も
	  * 同じものを２方向からPUSHした際に、ベクトル演算のようにはならない
	  * ただし２方向から同じマスにPUSHされた時は重なるので同時に判定されている
	* 今動いている途中かどうかを示すフラグが必要？
	  * moveした各objectを始点として、そこからPUSH可能なものが続く限りどちら方向に動くかフラグを立て、それらを順に解決？２方向からの場合どちらかが優先

     */

    for (auto& [obj_id, x, y] : you_ids){
	Object& srcObject = GetObject(obj_id, x, y);
	srcObject.SetDirection(dir); // Notes: Segmentation Fault
    }

    for (auto& [obj_id, x, y] : you_ids){
 	Object& srcObject = GetObject(obj_id, x, y);

	std::cout << "ProcessMoveByYou: (x, y, type, dir) = " 
		  << x << " " << y << " " 
		  << static_cast<int>(srcObject.GetType()) << " "
		  << static_cast<int>(srcObject.GetDirection())
		  << std::endl;


	if (!CanMove(x, y, dir, srcObject)) continue;
	std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);
	if ((x == _x) && (y == _y)) continue;

	auto dstObjects = m_map.At(_x, _y).GetObjects();

	for (auto & obj: dstObjects){
	    if (m_ruleManager.HasType(obj, m_map.At(_x, _y), m_map, ObjectType::PUSH)){
		ProcessPush(_x, _y, dir, obj);
	    }
	}

	std::cout << "<Before: AddObject> (type, dir) = " 
		  << static_cast<int>(srcObject.GetType()) << " "
		  << static_cast<int>(srcObject.GetDirection())
		  << std::endl;


	m_map.AddObject(_x, _y, srcObject);

	std::cout << "<After: AddObject> (type, dir) = " 
		  << static_cast<int>(srcObject.GetType()) << " "
		  << static_cast<int>(srcObject.GetDirection())
		  << std::endl;
	m_map.RemoveObject(x, y, srcObject);  // 先にremoveすると所有者が居なくなってまずいので後？
    }
    // std::exit(0);
    return;
}


void Game::ProcessPush(std::size_t x, std::size_t y, Direction dir,
		       Object& srcObject){

    std::cout << "<ProcessPush>: (x, y, type, dir) = " 
	      << x << " " << y << " " 
	      << static_cast<int>(srcObject.GetType()) << " "
	      << static_cast<int>(dir)
	      << std::endl;
    
    int _x;
    int _y;
    std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);

    auto dstSquare = m_map.At(_x, _y);
    auto dstObjects = dstSquare.GetObjects();

    for (auto & obj: dstObjects){
	if (m_ruleManager.HasType(obj, dstSquare, m_map, ObjectType::PUSH)){
	    ProcessPush(_x, _y, dir, obj);
	}
    }
    m_map.AddObject(_x, _y, srcObject);
    m_map.RemoveObject(x, y, srcObject);
    return;
}


void Game::UpdateObjects(){
    return;
}




void Game::CheckPlayState() // todo
{
    const auto youRules = m_ruleManager.GetRules(ObjectType::YOU);
    if (youRules.empty())
    {
        m_playState = PlayState::LOST;
        return;
    }

    auto you_ids = FindObjectIdsAndPositionsByProperty(ObjectType::YOU);
    if (you_ids.empty())
    {
        m_playState = PlayState::LOST;
        return;
    }

    /*
      Notes (letra418): 
      this judge is imcomplete and will be fixed after implementing conditional operators such as ON and FENCING.
     */

    // Player wins when an object has WIN at the same position as one of YOUs.
    // auto winRules = m_ruleManager.GetRules(ObjectType::WIN);

    for (auto& [_, x, y] : you_ids){
	auto square = m_map.At(x, y);
	for (auto& objOnPlayer: square.GetObjects()){
	    if (m_ruleManager.HasType(objOnPlayer, square, m_map, ObjectType::WIN)){
		m_playState = PlayState::WON;
	    }
	}
    }
}

// Instead of returning references to objects, this function returns tuples of (ObjectId, X, Y). This is due to  subsequent lost of references caused by memory reallocation of std::vector when an object is moved from a square to another square..
std::vector<PositionalObject> Game::FindObjectIdsAndPositionsByProperty(ObjectType property){
    std::vector<PositionalObject> res;

    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    std::cout << "<FindObjectsByProperty>" <<  std::endl;

    for (std::size_t y = 0; y < height; ++y){
        for (std::size_t x = 0; x < width; ++x){
	    Square& square = m_map.At(x, y); 
	    ObjectContainer& objs = square.GetVariableObjects();
	    for (auto itr = objs.begin(), e = objs.end(); itr != e; ++itr){ 
		if (m_ruleManager.HasType(*itr, m_map.At(x, y), m_map, property)){
		    std::tuple t = std::make_tuple(itr->GetId(), x, y);
		    // std::tuple t = std::tie(itr->GetId(), x, y);
		    res.emplace_back(t);
		}
	    }
        }
    }

    return res;
}

Object& Game::GetObject(std::size_t obj_id, std::size_t x, std::size_t y){
    Square& square = m_map.At(x, y);
    ObjectContainer& objs = square.GetVariableObjects();
    for (auto itr = objs.begin(), e = objs.end(); itr != e; ++itr){ 
	if (itr->GetId() == obj_id){
	    size_t index = std::distance(objs.begin(), itr);
	    Object& obj = objs.at(index); 
	    return obj;
	}
    }
    std::cout << "Exception: the object was not found (Id, x, y) =  "
	      << obj_id << " "
	      << x << " "
	      << y << " "
	      << std::endl;

    std::exit(EXIT_FAILURE);

}

} // namespace baba_is_auto

