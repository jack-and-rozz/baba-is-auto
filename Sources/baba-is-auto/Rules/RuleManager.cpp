// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <baba-is-auto/Rules/RuleManager.hpp>

#include <algorithm>
#include <tuple>

namespace baba_is_auto
{
void RuleManager::AddRule(const Rule& rule)
{
    m_rules.emplace_back(rule);
}

void RuleManager::RemoveRule(const Rule& rule)
{
    /*
      Note (letra418): 
      1. "Removing all the duplicated rules" / 2. "removing only one" should be distinguished?
     */
    const auto itr = std::find(m_rules.begin(), m_rules.end(), rule);
    if (itr != m_rules.end()){
        m_rules.erase(itr);
    }
}

void RuleManager::ClearRules()
{
    m_rules.clear();
}

std::vector<Rule> RuleManager::GetAllRules() const
{
    return m_rules;
}

std::vector<Rule> RuleManager::GetRules(ObjectType type) const
{
    std::vector<Rule> ret;

    for (auto& rule : m_rules)
    {
        if ((rule.GetSubject() == type) ||
            (rule.GetOperator() == type) ||
	    (rule.GetPredicate() == type))
        {
            ret.emplace_back(rule);
        }
    }

    return ret;
}

std::size_t RuleManager::GetNumRules() const
{
    return m_rules.size();
}


bool RuleManager::HasType(const Object& obj,ObjectType tgtType) const {
    auto objType = obj.GetType();
    objType = IsIconType(objType) ? ConvertIconToText(objType) : ObjectType::TEXT;
 
    // TEXT is always PUSH
    if ((objType == ObjectType::TEXT) && (tgtType == ObjectType::PUSH)){
	return true;
    }

    for (auto& rule : m_rules){
	if ((rule.GetSubject() == objType) &&
	    (rule.GetOperator() == ObjectType::IS) &&
	    (rule.GetPredicate() == tgtType)){
		return true;
	    }
    }
    return false;
}

}  // namespace baba_is_auto
