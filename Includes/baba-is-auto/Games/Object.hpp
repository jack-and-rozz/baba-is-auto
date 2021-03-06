// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef BABA_IS_AUTO_NOUN_HPP
#define BABA_IS_AUTO_NOUN_HPP

#include <baba-is-auto/Enums/GameEnums.hpp>


#include <algorithm>
#include <vector>
#include <iostream>
#include <unordered_set>


namespace baba_is_auto
{
//!
//! \brief Noun class.
//!
//! This class represents object such as noun, operator and property.
//! A noun is a word that corresponds to any possible in-game sprite. A few
//! nouns like STAR) have multiple corresponding sprites, while VIOLET and
//! FLOWER words have the same corresponding sprites. A noun can be used as a
//! NOUN IS VERB statement e.g. BABA IS YOU to give it a property or as a NOUN
//! IS NOUN statement e.g. WALL IS WATER to turn an object into another object.
//! An operator is a word that goes in between properties and nouns to show the
//! relation between them.
//! A property is something that can be attached to noun words to alter their
//! behavior.
//!

class Object
{
 public:
    Object() = default;
    explicit Object(ObjectType type, Direction dir=Direction::NONE); 

    bool operator==(const Object& rhs) const;

    ObjectType GetType() const;
    Direction GetDirection() const;
    // std::unordered_set<ObjectType> GetProperties() const;
    // bool HasProperty(ObjectType type) const;
    // void AddProperty(ObjectType type);
    // void RemoveProperty(ObjectType type);
    void SetDirection(Direction dir);

 private:
    // std::size_t m_id;
    ObjectType m_type;
    Direction m_direction;
    /* Notes (letra418):
       m_properties are Currently not used.
       Which is better properties are assigned to each object after parsing rules or to ask RuleManager whether an object has a property?
     */
    // std::unordered_set<ObjectType> m_properties;  

};

using ObjectContainer = std::vector<Object>;

class Square
{
 public:
    //! Default constructor.
    Square() = default;

    //! Constructs an object.
    //! \param types A list of object types.
    explicit Square(std::size_t x, std::size_t y, std::vector<Object> objects);

    //! Operator overloading for ==.
    //! \param rhs A right side of Object object.
    //! \return The value that indicates two objects are equal.
    // bool operator==(const Object& rhs) const;

    //! Adds an object type.
    //! \param type An object type to add.
    void AddObject(Object obj);

    //! Removes an object type.
    //! \param type An object type to remove.
    void RemoveObject(Object obj);

    void RemoveAllByType(ObjectType type);


    //! Checks the square has specific type.
    //! \param type An object type to check.
    //! \return The flag indicates that the object has specific type.
    /* 
       Note (letra418):  
       HasType does not support types induced by rules. 
       e.g., Even if there is a rule "WATER IS SINK" and ICON_WATER is at (x, y), 
       Map.At(x, y).HasType(ObjectType::SINK) returns false.
       In contrast, 
       RuleManager.HasType(object, ObjectType::SINK) 
       can return true.
     */ 
    bool HasType(ObjectType type) const;
    bool HasNounType() const;
    bool HasVerbType() const;
    bool HasPropertyType() const;
    bool HasTextType() const;

    bool isRule = false;
    ObjectContainer GetObjects() const;
    ObjectContainer GetObjectsByType(ObjectType type) const;
    ObjectContainer GetTextObjects() const;

    std::size_t X();
    std::size_t Y();

 private:
    std::size_t m_x;
    std::size_t m_y;
    ObjectContainer m_objects;

};

}  // namespace baba_is_auto

#endif
