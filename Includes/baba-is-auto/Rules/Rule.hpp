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

namespace baba_is_auto
{
//!
//! \brief Rule class.
//!
//! This class represents rule. The game centers around the manipulation of
//! "rules"--represented by tiles with words written on them--in order to allow
//! the titular character Baba (or some other object) to reach a specified goal.
//!
class Rule
{
 public:
    //! Constructs a rule.
    //! \param obj1 A first object.
    //! \param obj2 A second object.
    //! \param obj3 A third object.

    // Rule(TypeSequence types);
    Rule(ObjectType, ObjectType, ObjectType);

    //! Operator overloading for ==.
    //! \param rhs A right side of Rule object.
    //! \return The value that indicates two objects are equal.
    bool operator==(const Rule& rhs) const;

    std::tuple<ObjectType, ObjectType, ObjectType> objectTypes;
    //TypeSequence objectTypes;

    ObjectType GetSubject() const;
    ObjectType GetOperator() const;
    ObjectType GetPredicate() const;
    bool IsValid() const;
 private:
    //Rule* m_left = null;
    // std::unique_ptr<Rule> m_left;
    // std::unique_ptr<Rule> m_right;
    // ObjectType m_top;
    // bool m_is_valid;
    // void ParseTree(TypeSequence);
};


class RuleNode
{
 public:
    // RuleNode(ObjectType top);
    RuleNode(ObjectType top);
    RuleNode(ObjectType top, RuleNode left);
    RuleNode(ObjectType top, RuleNode left, RuleNode right);
    bool operator==(const ObjectType& type) const;
    bool HasTargetType(const ObjectType& type) const;
    bool SatisfyCondition(const Object& obj, const Map& map) const;
    // rule.HasTargetType(tgtType);
    // rule.SatisfyCondition(obj);

    ObjectType m_top;
 private:
    std::shared_ptr<RuleNode> m_left;
    std::shared_ptr<RuleNode> m_right;
 
};


}  // namespace baba_is_auto

#endif
