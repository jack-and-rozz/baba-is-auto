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
      Removing all the duplicated rules and removing only one should be distinguished?
     */
    const auto iter = std::find(m_rules.begin(), m_rules.end(), rule);
    if (iter != m_rules.end())
    {
        m_rules.erase(iter);
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

ObjectType RuleManager::FindPlayer() const
{
    /*
      Notes (letra418): 
      Currently only one type can be YOU.
     */

    for (auto& rule : m_rules)
    {
        if (rule.GetPredicate() == ObjectType::YOU)
        {
            const ObjectType type = rule.GetSubject();
            return ConvertTextToIcon(type);
        }
    }

    /*
      Notes (letra418): 
      This can cause bugs when "EMPTY IS YOU" is formed.
     */
    return ObjectType::ICON_EMPTY;
}


bool RuleManager::HasProperty(const ObjectType& type,
                              ObjectType property)
{
    auto _type = IsIconType(type) ? ConvertIconToText(type) : ObjectType::TEXT;

    // TEXT always has a property of PUSH
    if ((_type == ObjectType::TEXT) && (property == ObjectType::PUSH)){
	    return true;
	}
    for (auto& rule : m_rules){
	if ((rule.GetSubject() == _type) &&
	    (rule.GetPredicate() == property)){
		return true;
	    }
    }
    return false;
}

// bool RuleManager::HasProperty(const std::vector<ObjectType>& types,
//                               ObjectType property)
// {
//     for (auto type : types){
// 	type = IsIconType(type) ? ConvertIconToText(type) : ObjectType::TEXT;

// 	// TEXT always has a property of PUSH
// 	if ((type == ObjectType::TEXT) && (property == ObjectType::PUSH) )
// 	{
// 	    return true;
// 	}
// 	for (auto& rule : m_rules)
// 	    {
// 		if (std::get<0>(rule.objects).HasType(type) &&
// 		    std::get<2>(rule.objects).HasType(property))
// 		    {
// 			return true;
// 		    }
// 	    }
//     }
//     return false;
// }

}  // namespace baba_is_auto
