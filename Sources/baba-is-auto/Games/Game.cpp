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

    int _x;
    int _y;
    std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);

    // Check boundary
    if (_x < 0 || _x >= width || _y < 0 || _y >= height){
        return false;
    }

    auto dstSquare = m_map.At(_x, _y);
    const ObjectContainer dstObjects = dstSquare.GetObjects();

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

// void Game::ProcessMoveByYou(std::size_t x, std::size_t y, Direction dir,
// 			    Object srcObject)
void Game::ProcessMoveByYou(Direction dir)
{

    // Object* で返しているので，add/removeによってアドレスが変更されると無効なアドレスを指してしまう？
    auto players = FindObjectsByProperty(ObjectType::YOU);

    int _x;
    int _y;

    /*
      TODO (bug): 
      - 複数のYOUが並んでいるときにあるYOUが先に動いた結果，他のYOUをPUSHしてしまう
      - 複数のYOUが重なっている時に一番上のYOUが先に動いた結果，次のYOUが動く時に進路をブロックしてバラけてしまう
     */
    std::cout << "ProcessMoveByYou: main loop" << std::endl;
    for (auto& [x, y, srcObject] : players){
	// std::cout << static_cast<int>(srcObject.GetType()) << " "
	// 	  << static_cast<int>(srcObject.GetDirection()) << " "
	// 	  << x << " "
	// 	  << y << " "
	// 	  << std::endl;

	// std::cout << static_cast<int>(srcObject.GetDirection())
	// 	  << std::endl;
	srcObject->SetDirection(dir); // Notes: Segmentation Fault
	// srcObject->SetDirection(Direction::NONE); // Notes: Segmentation Fault

	if (!CanMove(x, y, dir, *srcObject)) continue;
	std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);
	if ((x == _x) && (y == _y)) continue;

	// auto square = m_map.At(_x, _y);
	// auto dstObjects = square.GetObjects();
	auto dstObjects = m_map.At(_x, _y).GetObjects();

	for (auto & obj: dstObjects){
	    if (m_ruleManager.HasType(obj, m_map.At(_x, _y), m_map, ObjectType::PUSH)){
		ProcessPush(_x, _y, dir, obj);
	    }
	}

	std::cout << "x, y = " << x << " " << y << std::endl;
	std::cout << "target YOU type: (type, dir)" 
		  << static_cast<int>(srcObject->GetType()) << " "
		  << static_cast<int>(srcObject->GetDirection()) 
		  << std::endl;

	m_map.AddObject(_x, _y, *srcObject);
	m_map.RemoveObject(x, y, *srcObject);  // 先にremoveすると所有者が居なくなってまずい？
    }
    // std::exit(0);
    return;
}

void Game::ProcessPush(std::size_t x, std::size_t y, Direction dir,
		       Object& srcObject){
    int _x;
    int _y;
    std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);

    auto dstSquare = m_map.At(_x, _y);
    // auto dstObjects = square.GetObjects();
    // const ObjectContainer dstObjects = dstSquare.GetObjects();
    auto dstObjects = dstSquare.GetObjects();

    // srcObject.SetDirection(dir);

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




void Game::CheckPlayState()
{
    const auto youRules = m_ruleManager.GetRules(ObjectType::YOU);
    if (youRules.empty())
    {
        m_playState = PlayState::LOST;
        return;
    }

    auto players = FindObjectsByProperty(ObjectType::YOU);
    if (players.empty())
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

    for (auto& [x, y, playerObj] : players){
	auto square = m_map.At(x, y);
	for (auto& objOnPlayer: square.GetObjects()){
	    if (m_ruleManager.HasType(objOnPlayer, square, m_map, ObjectType::WIN)){
		m_playState = PlayState::WON;
	    }
	}
    }
}

std::vector<std::tuple<size_t, size_t, Object*>> Game::FindObjectsByProperty(ObjectType property){
    std::vector<std::tuple<size_t, size_t, Object*>> res;

    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    std::cout << "FindObjectsByProperty" <<  std::endl;

    for (std::size_t y = 0; y < height; ++y){
        for (std::size_t x = 0; x < width; ++x){
 	    // ここで一時変数squareで参照を介するとアドレスがおかしくなる
	    Square& square = m_map.At(x, y); 
	    // ObjectContainer* objs = square.GetObjects2();

	    auto& objs = square.GetVariableObjects();

	    // こっちならOK
	    // ObjectContainer* objs = m_map.At(x, y).GetObjects2();
	    // std::cout << "typeid(square): " << typeid(square).name()  << std::endl;
	    // std::cout << "typeid(m_map.At(x,y)): " << typeid(m_map.At(x,y)).name()  << std::endl;
	    // std::cout << "Square: " << x << "," << y << " : " 
	    // 	      << &square << std::endl;
	    // std::cout << "m_map.At(x, y): " << x << "," << y << " : " 
	    // 	      << &(m_map.At(x, y)) << std::endl;

	    // for (auto& obj: objs){
	    //for (auto obj = objs->begin(), e = objs->end(); obj != e; ++obj){ 
	    for (auto obj = objs.begin(), e = objs.end(); obj != e; ++obj){ 
		if (m_ruleManager.HasType(*obj, m_map.At(x, y), m_map, property)){
		    // auto itr = std::find(objs.begin(), objs.end(), obj);
		    // std::cout << (itr == objs.end()) << std::endl;

		    std::cout << "x, y = " << x << " " << y << std::endl;
		    std::cout << "target YOU type: (type, dir)" 
			      << static_cast<int>(obj->GetType()) << " "
			      << static_cast<int>(obj->GetDirection()) 
			      << std::endl;
		    //size_t index = std::distance(objs->begin(), &obj);
		    // size_t index = std::distance(objs->begin(), obj);
		    size_t index = std::distance(objs.begin(), obj);

		    // ============= DEBUG =============== 
		    std::cout << "index, size = " 
		    	      << index << " " << objs.size()
		    	      << std::endl; 

		    std::cout << "target YOU type2: (type, dir)" 
		    	      << static_cast<int>(objs.at(index).GetType()) << " "
		    	      << std::endl;


		    // res.emplace_back(std::make_tuple(x, y, &obj));
		    // res.emplace_back(std::make_tuple(x, y, &(square.GetObjects2()->at(index))));
		    // res.emplace_back(std::make_tuple(x, y, &(objs->at(index))));
		    res.emplace_back(std::make_tuple(x, y, &(objs.at(index))));
		}
	    }
        }
    }

    std::cout << "res" << std::endl;
    for (auto& [xx, yy, player] : res){
		    std::cout << "x, y = " << xx << " " << yy << std::endl;
		    std::cout << "target YOU type: (type, dir)" 
			      << static_cast<int>(player->GetType()) << " "
			      << static_cast<int>(player->GetDirection()) 
			      << std::endl;
    }

    // ObjectContainer* objs00 = m_map.At(0,0).GetObjects2();
    // for (auto obj = objs00->begin(), e = objs00->end(); obj != e; ++obj){ 
    // 	std::cout << "At(0, 0)" << std::endl;
    // 	std::cout << "target YOU type: (type)"
    // 		  << static_cast<int>(obj->GetType()) << " "
    // 		  << std::endl;
    // }


    // for (std::size_t y = 0; y < height; ++y){
    //     for (std::size_t x = 0; x < width; ++x){
    // 	    ObjectContainer* objs = m_map.At(x,y).GetObjects2();
    // 	    for (auto obj = objs->begin(), e = objs->end(); obj != e; ++obj){ 
    // 		std::cout << "At(" << x <<"," << y << ")" << std::endl;
    // 		std::cout << "target YOU type: "
    // 			  << static_cast<int>(obj->GetType()) << " "
    // 			  << std::endl;
    // 	    }
    // 	}
    // }
    // std::exit(0);
    return res;
}

} // namespace baba_is_auto

