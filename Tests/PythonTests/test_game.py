"""
Copyright (c) 2020 Chris Ohk

I am making my contributions/submissions to this project solely in our
personal capacity and am not conveying any rights to any intellectual
property of any third parties.
"""

import pyBaba

def test_game_basic():
    game = pyBaba.Game("Resources/Maps/BabaIsYou.txt")
    assert game.GetMap().At(4, 1).HasType(pyBaba.ObjectType.ICON_BABA)
    assert game.GetMap().At(4, 9).HasType(pyBaba.ObjectType.ICON_FLAG)
    assert game.GetRuleManager().GetNumRules() == 4
    assert game.GetPlayerIcon() == pyBaba.ObjectType.ICON_BABA
    pos = game.GetMap().GetPositions(game.GetPlayerIcon())
    assert len(pos) == 1
    assert pos[0][0] == 4
    assert pos[0][1] == 1
    game.MovePlayer(pyBaba.Direction.UP)
    assert game.GetMap().At(3, 1).HasType(pyBaba.ObjectType.ICON_BABA)
    assert game.GetMap().At(4, 1).HasType(pyBaba.ObjectType.ICON_EMPTY)
    game.MovePlayer(pyBaba.Direction.UP)
    assert game.GetMap().At(3, 1).HasType(pyBaba.ObjectType.ICON_BABA)
    assert game.GetMap().At(2, 1).HasType(pyBaba.ObjectType.ICON_WALL)
    game.MovePlayer(pyBaba.Direction.RIGHT)
    game.MovePlayer(pyBaba.Direction.RIGHT)
    game.MovePlayer(pyBaba.Direction.RIGHT)
    assert game.GetMap().At(3, 4).HasType(pyBaba.ObjectType.ICON_BABA)
    assert game.GetMap().At(3, 3).HasType(pyBaba.ObjectType.ICON_TILE)
    game.MovePlayer(pyBaba.Direction.RIGHT)
    assert game.GetMap().At(3, 5).HasType(pyBaba.ObjectType.ICON_BABA)
    assert game.GetMap().At(3, 6).HasType(pyBaba.ObjectType.ICON_ROCK)
    assert game.GetMap().At(3, 4).HasType(pyBaba.ObjectType.ICON_TILE)
    game.MovePlayer(pyBaba.Direction.RIGHT)
    game.MovePlayer(pyBaba.Direction.DOWN)
    assert game.GetPlayState() == pyBaba.PlayState.PLAYING
    game.MovePlayer(pyBaba.Direction.RIGHT)
    game.MovePlayer(pyBaba.Direction.RIGHT)
    game.MovePlayer(pyBaba.Direction.RIGHT)
    assert game.GetPlayState() == pyBaba.PlayState.WON
    game.Reset()
    assert game.GetMap().At(4, 1).HasType(pyBaba.ObjectType.ICON_BABA)
    assert game.GetMap().At(4, 9).HasType(pyBaba.ObjectType.ICON_FLAG)
    assert game.GetRuleManager().GetNumRules() == 4
    assert game.GetPlayerIcon() == pyBaba.ObjectType.ICON_BABA
    assert game.GetPlayState() == pyBaba.PlayState.PLAYING

def test_game_lost():
    game = pyBaba.Game("Resources/Maps/SimpleMap.txt")
    assert game.GetMap().At(2, 0).HasType(pyBaba.ObjectType.ICON_BABA)
    assert game.GetRuleManager().GetNumRules() == 1
    assert game.GetPlayState() == pyBaba.PlayState.PLAYING
    game.MovePlayer(pyBaba.Direction.UP)
    assert game.GetRuleManager().GetNumRules() == 0
    assert game.GetPlayState() == pyBaba.PlayState.LOST