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

ObjectType Game::GetPlayerIcon() const
{
    return m_playerIcon;
}

void Game::MovePlayer(Direction dir)
{
    auto positions = GetMap().GetPositions(m_playerIcon);

    // 1. Movement
    for (auto& [x, y] : positions)
    {
	/*
	  TODO (letra418): 
	  Movements made by player and by a rule of MOVE are independently processed. This can cause different results from the original.
	  e.g.,) 
	  - We have "BABA IS YOU", "BABA IS MOVE", and "WALL IS STOP". 
	  - BABA is trying to move to a WALL that is two squares away.
	  - The movement fails in the original game, but BABA can move by one square in this implementation.
	  跳ね返ってる(YOUの移動+1, MOVEの移動-1)って考えればこの実装でも行ける？
	 */
	ProcessMoveByYou(x, y, dir, m_playerIcon);
    }

    ParseRules();
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

    m_playerIcon = m_ruleManager.FindPlayer();
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

        if (m_map.At(x, y).HasNounType() && m_map.At(x + 1, y).HasVerbType() &&
            (m_map.At(x + 2, y).HasNounType() ||
             m_map.At(x + 2, y).HasPropertyType()))
        {
            m_ruleManager.AddRule(
                { m_map.At(x, y), m_map.At(x + 1, y), m_map.At(x + 2, y) });

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
            m_ruleManager.AddRule(
                { m_map.At(x, y), m_map.At(x, y + 1), m_map.At(x, y + 2) });

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

bool Game::CanMove(std::size_t x, std::size_t y, Direction dir, 
		   ObjectType typeOfEntityOnSrc)
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

    const std::vector<ObjectType> typesOfEntitiesOnDst = m_map.At(_x, _y).GetTypes();

    for (auto & type: typesOfEntitiesOnDst){
	/*
	  TODO (letra418): 
	  implement SHUT, OPEN, PULL, WEAK, SWAP, FLOAT
	*/
	if (m_ruleManager.HasProperty(type, ObjectType::PUSH) && 
	    !CanMove(_x, _y, dir, type)){
	    return false;
	}
	else if (m_ruleManager.HasProperty(type, ObjectType::STOP)){
	    return false;
	}
    }
    return true;
}

void Game::ProcessMoveByYou(std::size_t x, std::size_t y, Direction dir,
			    ObjectType typeOfEntityOnSrc)
{

    if (!CanMove(x, y, dir, typeOfEntityOnSrc)) return;

    int _x;
    int _y;
    std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);

    if ((x == _x) && (y == _y)) return;

    const std::vector<ObjectType> typesOfEntitiesOnDst = m_map.At(_x, _y).GetTypes();
    for (auto & type: typesOfEntitiesOnDst){
	if (m_ruleManager.HasProperty(type, ObjectType::PUSH)){
	    ProcessPush(_x, _y, dir, type);
	}
    }

    m_map.AddObject(_x, _y, typeOfEntityOnSrc);
    m_map.RemoveObject(x, y, typeOfEntityOnSrc);
    return;

    // const std::vector<ObjectType> types = m_map.At(_x, _y).GetTypes();
    // // Textが他のタイルと重ならないことを想定している？
    // if (m_map.At(_x, _y).HasTextType()) 
    // {
    //     ProcessMove(_x, _y, dir, types[0]);
    // }
    // else if (m_ruleManager.HasProperty(types, ObjectType::PUSH))
    // {
    //     auto rules = m_ruleManager.GetRules(ObjectType::PUSH);

    //     for (auto& rule : rules)
    //     {
    //         const ObjectType nounType = std::get<0>(rule.objects).GetTypes()[0];
    //         ProcessMove(_x, _y, dir, ConvertTextToIcon(nounType)); 
    //     }
    // }

    // if ((x != _x) || (y != _y))
    // {
    // 	m_map.AddObject(_x, _y, type);
    // 	m_map.RemoveObject(x, y, type);
    // }

    // const std::vector<ObjectType> tgtTypesAfterMove = m_map.At(_x, _y).GetTypes();
    // if (m_ruleManager.HasProperty(tgtTypesAfterMove, ObjectType::SINK)){
    // 	for (auto &t: tgtTypesAfterMove){
    // 	  m_map.RemoveObject(_x, _y, t);
    // 	}
    // }
}

void Game::ProcessPush(std::size_t x, std::size_t y, Direction dir,
		 ObjectType typeOfEntityOnSrc){
    int _x;
    int _y;
    std::tie(_x, _y) = GetPositionsAfterMove(x, y, dir);

    const std::vector<ObjectType> typesOfEntitiesOnDst = m_map.At(_x, _y).GetTypes();
    for (auto & type: typesOfEntitiesOnDst){
	if (m_ruleManager.HasProperty(type, ObjectType::PUSH)){
	    ProcessPush(_x, _y, dir, type);
	}
    }
    m_map.AddObject(_x, _y, typeOfEntityOnSrc);
    m_map.RemoveObject(x, y, typeOfEntityOnSrc);
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

    auto positions = m_map.GetPositions(m_playerIcon);
    if (positions.empty())
    {
        m_playState = PlayState::LOST;
        return;
    }

    auto winRules = m_ruleManager.GetRules(ObjectType::WIN);
    for (auto& pos : positions)
    {
        for (auto& rule : winRules)
        {
            const ObjectType type = std::get<0>(rule.objects).GetTypes()[0];

            if (m_map.At(pos.first, pos.second)
                    .HasType(ConvertTextToIcon(type)))
            {
                m_playState = PlayState::WON;
            }
        }
    }
}
}  // namespace baba_is_auto
