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


// std::vector<Rule> RuleManager::GetRules(std::function<bool(Rule)> fn) const
// {
//     std::vector<Rule> ret;
//     for (auto itr = std::find_if(m_rules.begin(), m_rules.end(), fn);
// 	 itr != m_rules.end();
// 	 itr = std::find_if(++itr, m_rules.end(), fn))
// 	{
//             ret.emplace_back(itr);
// 	}
//     return ret;

    // for (auto& itr = std::find_if)
    // {
    //     if (fn(rule))
    //     {
    //         ret.emplace_back(rule);
    //     }
    // }

    // return ret;
// }


bool RuleManager::HasType(const Object& obj, ObjectType tgtType) const {
    auto objType = obj.GetType();
    objType = IsIconType(objType) ? ConvertIconToText(objType) : ObjectType::TEXT;

    // TEXT IS PUSH, LEVEL IS STOP, CURSOR IS SELECT, the three rules always implicitly exist.
    if ((objType == ObjectType::TEXT) && (tgtType == ObjectType::PUSH)){
	return true;
    }

    for (auto& rule : m_rules){
	if (IsPropertyType(tgtType)){
	    if ((rule.GetSubject() == objType) &&
		(rule.GetOperator() == ObjectType::IS) &&
		(rule.GetPredicate() == tgtType)){
		return true;
	    }
	}
	// else if (IsVerbType(tgtType)) {
	//     if ((rule.GetSubject() == objType) &&
	// 	(rule.GetOperator() == tgtType)){
	// 	return true;
	//     }
	// }
    }
    return false;
}


void RuleManager::ParseRules(Map& map)
{
    ClearRules();

    const std::size_t width = map.GetWidth();
    const std::size_t height = map.GetHeight();

    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            map.At(x, y).isRule = false;
        }
    }

    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {
            ParseRule(map, x, y, RuleDirection::HORIZONTAL);
            ParseRule(map, x, y, RuleDirection::VERTICAL);
        }
    }

    // m_playerIcons = m_ruleManager.GetSubjectsByPredicate(ObjectType::YOU);
}

void RuleManager::ParseRule(Map& map, std::size_t x, std::size_t y, RuleDirection direction)
{
    const std::size_t width = map.GetWidth();
    const std::size_t height = map.GetHeight();

    if (direction == RuleDirection::HORIZONTAL)
    {
        if (x + 2 >= width)
        {
            return;
        }
	// Currently an edge case is ignored where two texts are overlapping.

        if (map.At(x, y).HasNounType() && map.At(x + 1, y).HasVerbType() &&
            (map.At(x + 2, y).HasNounType() ||
             map.At(x + 2, y).HasPropertyType()))
	{
	    auto type1 = map.At(x, y).GetTextObjects()[0].GetType();
	    auto type2 = map.At(x + 1, y).GetTextObjects()[0].GetType();
	    auto type3 = map.At(x + 2, y).GetTextObjects()[0].GetType();

	    Rule newRule = Rule(type1, type2, type3);
	    AddRule(newRule);

	    map.At(x, y).isRule = true;
	    map.At(x + 1, y).isRule = true;
	    map.At(x + 2, y).isRule = true;
	}
    }
    else if (direction == RuleDirection::VERTICAL)
    {
        if (y + 2 >= height)
        {
            return;
        }

        if (map.At(x, y).HasNounType() && map.At(x, y + 1).HasVerbType() &&
            (map.At(x, y + 2).HasNounType() ||
             map.At(x, y + 2).HasPropertyType()))
	{
	    auto type1 = map.At(x, y).GetTextObjects()[0].GetType();
	    auto type2 = map.At(x, y + 1).GetTextObjects()[0].GetType();
	    auto type3 = map.At(x, y + 2).GetTextObjects()[0].GetType();

	    Rule newRule = Rule(type1, type2, type3);
	    AddRule(newRule);

            map.At(x, y).isRule = true;
            map.At(x, y + 1).isRule = true;
            map.At(x, y + 2).isRule = true;
	}
    }
}


}  // namespace baba_is_auto


/*
どういうruleの持ち方をすべきだろうか？

<現在のRule, RuleMangerについて>
- Ruleは ObjectType (Noun, Op, Verb, Property) を要素として持つtripleのwrapper. 修飾子は未実装。
- RuleManagerはそのvectorのwrapper.
- ルールのparseはGame::ParseRules
- あるObjectTypeに関してルールが存在するかの判定はRuleManager::HasType

- ルール自体の適用はGameクラス内の各処理でHasTypeを用いて条件分岐し、直接書いている
  * Noun, PropertyなどのType自体をクラスに分割し、関数処理としてルールの実行を適用可能か？
  * ルール自体はTypeの組み合わせとなるため、PropertyではなくRule側にapplyを書く必要がある
    * 実際の効果自体はPropertyの方に実装する必要がある。
     * 例えば、nA EAT nB などのVerbがメインのタイプならEATのsrc/tgをmapから探してきてverb.apply(nA, nB) のような形？
     * Propertyが主導となるタイプ nA IS PROP は prop.apply(nA) となる。ただ、この処理は独立には不可能じゃないか？
       * 実行順、同時解決の問題がある
       * PUSHのようにYOU, MOVEから誘発される処理はどうなる？
       * 結局現状のように、各ルールの適用は関数として直接書くのが無難か



*/
