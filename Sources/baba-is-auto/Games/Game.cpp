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
    ProcessMoveByYou(dir);

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
    auto dstObjects = dstSquare.GetObjects();

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

    auto players = FindObjectsByProperty(ObjectType::YOU);

    int _x;
    int _y;

    /*
      TODO (bug): 
      - 複数のYOUが並んでいるときにあるYOUが先に動いた結果，他のYOUをPUSHしてしまう
      - 複数のYOUが重なっている時に一番上のYOUが先に動いた結果，次のYOUが動く時に進路をブロックしてバラけてしまう
     */

    for (auto& [x, y, srcObject] : players){
	// srcObject.SetDirection(dir);

	if (!CanMove(x, y, dir, srcObject)) continue;
	std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);
	if ((x == _x) && (y == _y)) continue;

	auto square = m_map.At(_x, _y);
	auto dstObjects = square.GetObjects();

	for (auto & obj: dstObjects){
	    if (m_ruleManager.HasType(obj, square, m_map, ObjectType::PUSH)){
		ProcessPush(_x, _y, dir, obj);
	    }
	}

	m_map.RemoveObject(x, y, srcObject);
	m_map.AddObject(_x, _y, srcObject);

    }
    return;
}

void Game::ProcessPush(std::size_t x, std::size_t y, Direction dir,
		       Object& srcObject){
    int _x;
    int _y;
    std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);

    auto square = m_map.At(_x, _y);
    auto dstObjects = square.GetObjects();

    // srcObject.SetDirection(dir);

    for (auto & obj: dstObjects){
	if (m_ruleManager.HasType(obj, square, m_map, ObjectType::PUSH)){
	    ProcessPush(_x, _y, dir, obj);
	}
    }
    m_map.RemoveObject(x, y, srcObject);
    m_map.AddObject(_x, _y, srcObject);
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
    // for (auto& rule : winRules){
    //     const ObjectType winType = rule.GetSubject();
    //     if (square.HasType(ConvertTextToIcon(winType))){
    //         m_playState = PlayState::WON;
    //     }
    // }
}

std::vector<std::tuple<size_t, size_t, Object>> Game::FindObjectsByProperty(ObjectType property) const{
    std::vector<std::tuple<size_t, size_t, Object>> res;

    const std::size_t width = m_map.GetWidth();
    const std::size_t height = m_map.GetHeight();

    for (std::size_t y = 0; y < height; ++y){
        for (std::size_t x = 0; x < width; ++x){
	    auto square = m_map.At(x, y);
	    for (auto& obj: square.GetObjects()){
		if (m_ruleManager.HasType(obj, square, m_map, property)){
		    res.emplace_back(std::make_tuple(x, y, obj));
		}
	    }
        }
    }
    return res;
}

} // namespace baba_is_auto

