// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef BABA_IS_AUTO_GAME_HPP
#define BABA_IS_AUTO_GAME_HPP

#include <baba-is-auto/Games/Map.hpp>
#include <baba-is-auto/Rules/RuleManager.hpp>

#include <string>
#include <iterator>
#include <iostream>
#include <typeinfo>
#include <tuple>
#include <random>

namespace baba_is_auto
{
//!
//! \brief Game class.
//!
//! This class represents game. A game is a structured form of Baba Is You.
//!
class Game
{
 public:
    //! Constructs game with given \p mapFileName.
    //! \param filename The file name to load a map.
    explicit Game(std::string_view filename);

    //! Resets map and rule data.
    void Reset();

    //! Gets a map object.
    //! \return A map object.
    Map& GetMap();

    //! Gets a map object.
    //! \return A map object.
    const Map& GetMap() const;

    //! Gets a rule manager object.
    //! \return A rule manager object.
    RuleManager& GetRuleManager();

    //! Gets the play state of the game.
    //! \return The play state of the game.
    PlayState GetPlayState() const;

    //! Gets an icon type that represents player.
    //! \return An icon type that represents player.
    // ObjectType GetPlayerIcons() const;

    //! Moves the icon that represents player.
    //! \param dir The direction to move the player.
    void MovePlayer(Direction dir);

    // Object& GetObject(std::size_t obj_id, std::size_t x, std::size_t y);
    std::vector<PositionalObjectId> FindObjectIdsAndPositionsByProperty(ObjectType property);
    inline std::vector<PositionalObjectId> FindObjectIdsAndPositionsByNoun(ObjectType noun);
    std::vector<std::tuple<PositionalObjectId>, TypeSequence> FindObjectIdsAndPositionsAndTargetsByVerb(ObjectType objtype);


    int RandInt(int min, int max);

 private:
    bool CanMove(std::size_t x, std::size_t y, Direction dir, const Object& obj) const;

    void ProcessYOU(Direction dir);
    void ProcessMOVE();
    void ProcessIS();
    void ProcessSHIFT();

    bool ProcessSINK();
    bool ProcessHOTAndMELT();
    bool ProcessDEFEAT();

    //! Checks the play state of the game.
    void CheckPlayState();
    void SetPushedDirToObjects(std::size_t x, std::size_t y, Direction dir);
    Direction SetRandomDirectionToObject(Object&);
    void ResolveAllMoveFlags();
    void ResolveAllRemoveFlags();
    void ResolveAllChangeFlags();

    Map m_map;
    RuleManager m_ruleManager;
    std::mt19937 mt;

    PlayState m_playState = PlayState::INVALID;
    // std::vector<ObjectType> m_playerIcons;
};
}  // namespace baba_is_auto

#endif
