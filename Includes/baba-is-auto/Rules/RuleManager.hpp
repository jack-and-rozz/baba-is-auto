// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#ifndef BABA_IS_AUTO_RULE_MANAGER_HPP
#define BABA_IS_AUTO_RULE_MANAGER_HPP


#include <baba-is-auto/Enums/RuleEnums.hpp>
#include <baba-is-auto/Rules/Rule.hpp>
#include <baba-is-auto/Games/Map.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <optional>

namespace baba_is_auto
{
//!
//! \brief RuleManager class.
//!
//! This class manages a list of rules.
//!
using Grammar = std::tuple<TypeSequence, ObjectType>;
class RuleManager
{
 public:
    RuleManager();
    //! Adds a rule.
    //! \param rule The rule.
    //void AddRule(const Rule& rule);
    void AddRule(const RuleNode& rule);

    //! Removes a rule.
    //! \param rule The rule.
    void RemoveRule(const RuleNode& rule);

    //! Clears a list of rules.
    void ClearRules();

    //! Gets a list of rules that has specific type.
    //! \param type The object type to find a rule.
    //! \return A list of rules that has specific type.
    std::vector<RuleNode> GetRules(ObjectType type) const;
    std::vector<RuleNode> GetAllRules() const;

    //! Gets the number of rules.
    //! \return The number of rules.
    std::size_t GetNumRules() const;

    //! Checks an object has specific property.
    //! \param types A list of object types to check it has property.
    //! \param property The property to check.
    //! \return The flag indicates that an object has specific property.

    bool HasType(const Object& obj, const Map& map, ObjectType type) const;
    void ParseRules(Map& map);
    void ParseRule(Map& map, std::size_t x, std::size_t y, RuleDirection direction);
    std::optional<RuleNode> BuildRuleTree(TypeSequence seq);
 private:
    std::vector<RuleNode> m_rules;
    std::vector<Grammar> m_grammars;
    // TypeSequence GetAllNouns();
    // TypeSequence GetAllGenVerbs();
    // TypeSequence GetAllProperties();
    // TypeSequence GetAllPreModifiers();
    // TypeSequence GetAllPostModifiers();
};
    void DbgPrint(std::string title, TypeSequence types);
}  // namespace baba_is_auto

#endif
