// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <baba-is-auto/Games/Map.hpp>

#include <fstream>

namespace baba_is_auto
{
Map::Map(std::size_t width, std::size_t height)
    : m_width(width), m_height(height)
{
    m_initSquares.reserve(m_width * m_height);
    m_squares.reserve(m_width * m_height);

    // for (std::size_t i = 0; i < m_width * m_height; ++i)
    // {
    // 	std::cout <<  "Square sq = Square();"  << std::endl;

    // 	Square sq = Square();

    // 	std::cout <<  "m_initSquares.emplace_back(sq);"  << std::endl;
    // 	m_initSquares.emplace_back(sq);
    // 	m_squares.emplace_back(Square(sq));
    //     // m_initObjects.emplace_back(
    //     //     std::vector<ObjectType>{ ObjectType::ICON_EMPTY });
    //     // m_objects.emplace_back(
    //     //     std::vector<ObjectType>{ ObjectType::ICON_EMPTY });
    // }
}

void Map::Load(std::string_view filename)
{
    std::ifstream mapFile(filename.data());

    mapFile >> m_width >> m_height;

    int val = 0;

    /* Notes (letra418):
       TODO: load direction?
    */
    for (std::size_t y = 0; y < m_height; ++y){
	for (std::size_t x = 0; x < m_width; ++x){

        mapFile >> val;

	Object obj = Object(static_cast<ObjectType>(val));
	ObjectContainer init_vector{obj};
	Square sq = Square(x, y, init_vector);

	m_initSquares.emplace_back(sq);
	m_squares.emplace_back(sq);
        // m_initObjects.emplace_back(
        //     std::vector<ObjectType>{ static_cast<ObjectType>(val) });
        // m_objects.emplace_back(
        //     std::vector<ObjectType>{ static_cast<ObjectType>(val) });
	}
    }
}

void Map::Reset()
{
    m_squares = m_initSquares;
}

void Map::AddObject(std::size_t x, std::size_t y, const Object& obj)
{
    m_squares.at(y * m_width + x).AddObject(obj);
}

void Map::RemoveObject(std::size_t x, std::size_t y, const Object& obj)
{
    m_squares.at(y * m_width + x).RemoveObject(obj);
}


Square& Map::At(std::size_t x, std::size_t y)
{
    return m_squares.at(y * m_width + x);
}

const Square& Map::At(std::size_t x, std::size_t y) const
{
    return m_squares.at(y * m_width + x);
}

Object& Map::GetObject(ObjectId obj_id, std::size_t x, std::size_t y){
    Square& square = At(x, y);
    ObjectContainer& objs = square.GetObjects();
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

const ObjectContainer& Map::GetObjects(std::size_t x, std::size_t y) const {
    const ObjectContainer& objs = At(x, y).GetObjects();
    return objs;
}

ObjectContainer& Map::GetObjects(std::size_t x, std::size_t y){
    ObjectContainer& objs = At(x, y).GetObjects();
    return objs;
}




}  // namespace baba_is_auto
