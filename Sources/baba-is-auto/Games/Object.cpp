// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <baba-is-auto/Games/Object.hpp>

#include <algorithm>

namespace baba_is_auto
{

/******************************************
                Object
*******************************************/
// Object::Object(std::size_t id, ObjectType type, Direction dir=Direction::NONE){
Object::Object(ObjectType type, Direction dir){	
    // m_id = id;
    m_type = type;
    m_direction = dir;
}

bool Object::operator==(const Object& rhs) const
{
    // 重複しないIDを振った方が確実？
    // return (m_id == rhs.m_id) && (m_type == rhs.m_type) && (m_direction == rhs.m_direction);
    return (m_type == rhs.m_type) && (m_direction == rhs.m_direction);
}


ObjectType Object::GetType() const{
    return m_type;
}

Direction Object::GetDirection() const{
    return m_direction;
}


// std::unordered_set<ObjectType> Object::GetProperties() const{
//     return m_properties;
// }

// bool Object::HasProperty(ObjectType type) const{
//     return m_properties.find(type) != m_properties.end();
// }

// void Object::AddProperty(ObjectType type){
//     m_properties.insert(type);
// }

// void Object::RemoveProperty(ObjectType type){
//     m_properties.erase(type);
// }

void Object::SetDirection(Direction dir){
    m_direction = dir;
}

/******************************************
               Square
*******************************************/


Square::Square(ObjectContainer objects) : m_objects(std::move(objects))
{
}


void Square::AddObject(Object object){
    m_objects.emplace_back(object);

    RemoveAllByType(ObjectType::ICON_EMPTY);
}

void Square::RemoveObject(Object object){
    const auto itr = std::find(m_objects.begin(), m_objects.end(), object);
    m_objects.erase(itr);

    if (m_objects.empty()){
	Object empty = Object(ObjectType::ICON_EMPTY);
	m_objects.emplace_back(empty);
    }
}

void Square::RemoveAllByType(ObjectType type){
    // std::erase_if(m_objects.begin(), m_objects.end(),
    // 		  [&](Object x){
    // 		      return x.GetType() == type; 
    // 		  });
    auto itr = std::remove_if(m_objects.begin(), m_objects.end(), 
    			      [&](Object x){
    				  return x.GetType() == type; 
    			      });
    m_objects.erase(itr, m_objects.end());
}


ObjectContainer Square::GetObjects() const{
    return m_objects;
}

ObjectContainer Square::GetObjectsByType(ObjectType type) const{
    ObjectContainer res;
    for (auto& obj: m_objects){
	if (obj.GetType() == type){
	    res.emplace_back(obj);
	}
    }
    return res;
}

bool Square::HasType(ObjectType type) const
{
    auto itr = std::find_if(m_objects.begin(), m_objects.end(), 
			    [&](Object x) { 
				return (x.GetType() == type);
			    });
    return itr != m_objects.end();
}


bool Square::HasTextType() const
{
    for (auto& obj : m_objects)
    {
        if (!IsIconType(obj.GetType()))
        {
            return true;
        }
    }
    return false;
}

bool Square::HasNounType() const
{
    for (auto& obj : m_objects)
    {
        if (IsNounType(obj.GetType()))
        {
            return true;
        }
    }

    return false;
}

bool Square::HasVerbType() const
{
    for (auto& obj : m_objects)
    {
        if (IsVerbType(obj.GetType()))
        {
            return true;
        }
    }

    return false;
}

bool Square::HasPropertyType() const
{
    for (auto& obj : m_objects)
    {
        if (IsPropertyType(obj.GetType()))
        {
            return true;
        }
    }

    return false;
}


ObjectContainer Square::GetTextObjects() const
{
    std::vector<Object> objects;
    for (auto& obj : m_objects)
    {
	auto type = obj.GetType();
        if (!IsIconType(type))
        {
            objects.emplace_back(type);
        }
    }
    return objects;
}

}  // namespace baba_is_auto

