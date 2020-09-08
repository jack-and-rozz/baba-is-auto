// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <baba-is-auto/Rules/Rule.hpp>

namespace baba_is_auto
{
Rule::Rule(ObjectType type1, ObjectType type2, ObjectType type3)
{
    objectTypes = { type1, type2, type3 };
}

bool Rule::operator==(const Rule& rhs) const
{
    return objectTypes == rhs.objectTypes;
}

ObjectType Rule::GetSubject() const
{
    return std::get<0>(objectTypes);
}
ObjectType Rule::GetOperator() const
{
    return std::get<1>(objectTypes);
}
ObjectType Rule::GetPredicate() const
{
    return std::get<2>(objectTypes);
}

}  // namespace baba_is_auto
