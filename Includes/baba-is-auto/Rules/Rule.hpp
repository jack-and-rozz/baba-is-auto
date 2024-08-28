// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef BABA_IS_AUTO_RULE_HPP
#define BABA_IS_AUTO_RULE_HPP

#include <baba-is-auto/Games/Object.hpp>
#include <baba-is-auto/Games/Map.hpp>

#include <tuple>
#include <valarray>
#include <memory>
#include <algorithm>


namespace baba_is_auto
{

class RuleNode
{
 public:
    // RuleNode(ObjectType top);
    RuleNode(ObjectType top);
    RuleNode(ObjectType top, RuleNode left);
    RuleNode(ObjectType top, RuleNode left, RuleNode right);
    bool operator==(const ObjectType& type) const;
    //bool HasTargetType(const ObjectType& type) const;
    bool IsSubjectConditionSatisfied(const Object& obj, const Map& map) const;
    const RuleNode& GetSubject() const;
    ObjectType GetVerb() const;
    const RuleNode& GetPredicate() const;
    TypeSequence GetLeaves() const;
    TypeSequence ParseNPtoTypes() const;

    // rule.HasTargetType(tgtType);
    // rule.SatisfyCondition(obj);
    //TypeSequence RuleNote::DecomposeNPIntoTypes() const;
    ObjectType m_top;
 private:
    std::shared_ptr<RuleNode> m_left;
    std::shared_ptr<RuleNode> m_right;

    bool ParseNOTtoBool() const;
 
};


}  // namespace baba_is_auto

#endif
