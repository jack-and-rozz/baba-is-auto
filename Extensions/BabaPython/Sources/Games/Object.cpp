// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <Games/Object.hpp>
#include <baba-is-auto/Games/Object.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace baba_is_auto;

void AddObject(pybind11::module& m)
{
    pybind11::class_<Object>(m, "Object")
        .def(pybind11::init<>())
        .def(pybind11::init<ObjectType, Direction>())
        .def("__eq__",
             [](const Object& left, const Object& right) {
                 return ((left.GetType() == right.GetType()) &&
			 (left.GetDirection() == right.GetDirection()));
             })
	.def("GetType", &Object::GetType)
	.def("GetDirection", &Object::GetDirection);

    pybind11::class_<Square>(m, "Square")
        .def(pybind11::init<>())
        .def(pybind11::init<std::size_t, std::size_t, ObjectContainer>())
        .def("GetObjects", &Square::GetObjects)
        .def("HasType", &Square::HasType)
        .def("HasTextType", &Square::HasTextType)
        .def("HasNounType", &Square::HasNounType)
        .def("HasVerbType", &Square::HasVerbType)
        .def("HasPropertyType", &Square::HasPropertyType);

}
