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
Object::Object(ObjectType type, Direction dir){	
    // m_id = id;
    m_type = type;
    m_direction = dir;
}

bool Object::operator==(const Object& rhs) const
{
    return (m_type == rhs.m_type) && (m_direction == rhs.m_direction);
}


ObjectType Object::GetType() const{
    return m_type;
}

Direction Object::GetDirection() const{
    return m_direction;
}

void Object::SetType(ObjectType type){
    m_type = type;
}

void Object::SetDirection(Direction dir){
    m_direction = dir;
}


/******************************************
               Square
*******************************************/
/*
  This is a class to manage objects on a square of a map.
*/

Square::Square(std::size_t x, std::size_t y, ObjectContainer objects) 
    : m_x(x), m_y(y), m_objects(std::move(objects))
{
}


std::size_t Square::X(){return m_x;}
std::size_t Square::Y(){return m_y;}

void Square::AddObject(const Object& object){
    std::cout << "<AddObject>" << std::endl;
    std::cout << "(x, y, obj_type) = "
	      << m_x << " " << m_y << " " << static_cast<int>(object.GetType())
	      << std::endl;
    m_objects.emplace_back(object);
    RemoveAllByType(ObjectType::ICON_EMPTY);
}

void Square::RemoveObject(const Object& object){
    std::cout << "<RemoveObject>" << std::endl;
    std::cout << "(x, y, obj_type) = "
	      << m_x << " " << m_y << " " << static_cast<int>(object.GetType())
	      << std::endl;

    const auto itr = std::find(m_objects.begin(), m_objects.end(), object);

    if (itr == m_objects.end()){
	std::cout << "Exception: the object to be removed is not found in m_objects." << std::endl;
	std::cout << "Candidate" << std::endl;

	for (auto& obj: m_objects){
	    std::cout << "(obj_type, obj_dir) = "
		      << static_cast<int>(obj.GetType()) 
		      << " "
		      << static_cast<int>(obj.GetDirection())
		      << std::endl;
	}
	std::cout << "Target Object" << std::endl;

	std::cout << "(obj_type, obj_dir) = "
		  << static_cast<int>(object.GetType()) 
		  << " "
		  << static_cast<int>(object.GetDirection())
		  << std::endl;

	std::exit(EXIT_FAILURE);
    }

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
    // As the removed elements still exist at the end of the container, [itr, m_object.end()) needs to be cut off.
    m_objects.erase(itr, m_objects.end()); 
}


// const ObjectContainer& Square::GetObjects() const{
const ObjectContainer& Square::GetObjects() const{
    return m_objects;
}

ObjectContainer& Square::GetVariableObjects(){
    return m_objects;
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

