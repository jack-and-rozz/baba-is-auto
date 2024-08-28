// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <Rules/Rule.hpp>
#include <baba-is-auto/Rules/Rule.hpp>

#include <pybind11/pybind11.h>

using namespace baba_is_auto;

void AddRule(pybind11::module& m)
{
    // pybind11::class_<Rule>(m, "Rule")
    //     .def(pybind11::init<Object, Object, Object>())
    //     .def("__eq__",
    //          [](const Rule& left, const Rule& right) { return left == right; });

    pybind11::class_<RuleNode>(m, "RuleNode")
        .def(pybind11::init<ObjectType, ObjectType, ObjectType>())
        .def("__eq__",
             [](const RuleNode& left, const RuleNode& right) { return left == right; })
	.def("GetLeaves", &RuleNode::GetLeaves);
	// .def("GetSubject", &RuleNode::GetSubject)
	// .def("GetVerb", &RuleNode::GetVerb)
	// .def("GetComplement", &RuleNode::GetComplement);
}
