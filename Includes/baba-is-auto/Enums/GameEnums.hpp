// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef BABA_IS_AUTO_GAME_ENUMS_HPP
#define BABA_IS_AUTO_GAME_ENUMS_HPP

#include <vector>

namespace baba_is_auto
{
//! \brief An enumerator for identifying the object.
enum class ObjectType
{
    NOUN_TYPE,
#define X(a) a,
#include "NounType.def"
#undef X
    OP_TYPE,
#define X(a) a,
#include "OpType.def"
#undef X
    PROPERTY_TYPE,
#define X(a) a,
#include "PropertyType.def"
#undef X
    ICON_TYPE,
#define X(a) a,
#include "IconType.def"
#undef X
    GRAMMAR_TYPE,
#define X(a) a,
#include "GrammarType.def"
#undef X
};

using TypeSequence = std::vector<ObjectType>;

//! Checks \p type is text type.
//! \param type The object type.
//! \return The flag that indicates it is text type.
constexpr bool IsTextType(ObjectType type)
{
    return type < ObjectType::ICON_TYPE;
}

constexpr bool IsNounType(ObjectType type)
{
    return (type > ObjectType::NOUN_TYPE && type < ObjectType::OP_TYPE);
}

constexpr bool IsOpType(ObjectType type)
{
    return (type > ObjectType::OP_TYPE && type < ObjectType::PROPERTY_TYPE);
}

constexpr bool IsVerbType(ObjectType type)
{
    return (type == ObjectType::IS || type == ObjectType::HAS ||
            type == ObjectType::MAKE);
}

constexpr bool IsPropertyType(ObjectType type)
{
    return (type > ObjectType::PROPERTY_TYPE && type < ObjectType::ICON_TYPE);
}

constexpr bool IsIconType(ObjectType type)
{
    return (type > ObjectType::ICON_TYPE && type < ObjectType::GRAMMAR_TYPE);
}

constexpr bool IsGrammarType(ObjectType type)
{
    return (type > ObjectType::GRAMMAR_TYPE);
}

constexpr bool IsAND(ObjectType type)
{
    return (type == ObjectType::AND);
}

constexpr bool IsNOT(ObjectType type)
{
    return (type == ObjectType::NOT);
}


constexpr bool IsPreModifierType(ObjectType type)
{
    return (type == ObjectType::LONELY);
}

constexpr bool IsPostModifierType(ObjectType type)
{
    return (type == ObjectType::ON || type == ObjectType::NEAR ||
	    type == ObjectType::FACING);
}


// constexpr int NumNouns()
// {
//     return static_cast<int>(ObjectType::OP_TYPE) - static_cast<int>(ObjectType::NOUN_TYPE) - 1;
// }

// constexpr int NumProperties()
// {
//     return static_cast<int>(ObjectType::ICON_TYPE) - static_cast<int>(ObjectType::PROPERTY_TYPE) - 1;
// }


//! Converts icon type to text type.
//! \param type The icon type to convert.
//! \return The converted text type.
constexpr ObjectType ConvertIconToText(ObjectType type)
{
    const auto typeVal = static_cast<int>(type);
    const auto iconTypeVal = static_cast<int>(ObjectType::ICON_TYPE);

    if (typeVal <= iconTypeVal)
    {
        return type;
    }

    const int convertedVal = typeVal - iconTypeVal;
    return static_cast<ObjectType>(convertedVal);
}

//! Converts text type to icon type.
//! \param type The text type to convert.
//! \return The converted icon type.
constexpr ObjectType ConvertTextToIcon(ObjectType type)
{
    const auto typeVal = static_cast<int>(type);
    const auto iconTypeVal = static_cast<int>(ObjectType::ICON_TYPE);

    if (typeVal > iconTypeVal)
    {
        return type;
    }

    const int convertedVal = typeVal + iconTypeVal;
    return static_cast<ObjectType>(convertedVal);
}

//! \brief An enumerator for identifying the play state.
enum class PlayState
{
    INVALID,
    PLAYING,
    WON,
    LOST
};

//! \brief An enumerator for identifying the direction.
enum class Direction
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};


TypeSequence GetAllNouns()
{
    TypeSequence s;
    int begin = static_cast<int>(ObjectType::NOUN_TYPE) + 1;
    int end = static_cast<int>(ObjectType::OP_TYPE);
    for (int i=begin; i<end; ++i){
	s.emplace_back(static_cast<ObjectType>(i));
    }
    return s;
}
TypeSequence GetAllProperties()
{
    TypeSequence s;
    int begin = static_cast<int>(ObjectType::PROPERTY_TYPE) + 1;
    int end = static_cast<int>(ObjectType::ICON_TYPE);
    for (int i=begin; i<end; ++i){
	s.emplace_back(static_cast<ObjectType>(i));
    }
    return s;
}

TypeSequence GetAllGenVerbs()
{
    TypeSequence s{ObjectType::HAS, ObjectType::MAKE};
    return s;
}

TypeSequence GetAllPreModifiers()
{
    TypeSequence s{ObjectType::LONELY};
    return s;
}
TypeSequence GetAllPostModifiers()
{
    TypeSequence s{ObjectType::ON, ObjectType::NEAR, ObjectType::FACING};
    return s;
}

}  // namespace baba_is_auto

#endif  // BABA_IS_AUTO_WORD_ENUMS_HPP
