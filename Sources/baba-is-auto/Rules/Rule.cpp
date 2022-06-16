// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

/* 
memo: letra418

<Parsing priority>
- Noun&Property < NOT < AND < Pre-Modifier < Post-Modifier < Verb

<Modifier + not の範囲、パースの順番>
- (((not baba) and flag) facing (keke and door)) is stop が成立
  * notの範囲は直前1語, and よりパースが先
  * and のパースはModifierより先
  * lonely, facing, near, on などは並列句全体に掛かる


<andが絡んだ時のPreM, PostMの挙動>
# 1. ANDの左右でPostMのみ
- "(baba near wall) and keke is X" は不成立
- "baba near (wall and keke) is X" は成立
- andの左右を入れ替えても同様
- "keke and (baba near wall) is X" は不成立
- "(keke and baba) near wall is X" は成立
  * このため、(NP PostM NP) AND NP というパターンは無い
  * おなじく、NP AND (NP PostM NP) というパターンも無い。
  * 結局、ANDの右辺にPostMは存在し得るが、その場合でもパースの順序は後になる。

# 2. ANDの左右でPreMのみ
- "(baba and lonely door) is X" は不成立
  * "NP AND PreM NP" というパターンは無い
  * PostMと違い、そもそもANDの右辺に存在し得ない
- "(lonely baba) and door is X" は不成立
- "lonely (baba and door) is X" は成立
  * (PreM NP) AND NP というパターンは無い -> 前述した通りANDが優先


# 3. PreMとPostMの複合
- "baba near wall and lonely keke is X" は不成立
- "baba near wall and keke on door is X" は不成立
- "lonely (baba and keke) near wall is X" は成立 -> 1. 2.で成立しうる形の複合だが、PreM, PostMはANDとは独立


> 結局、AND句の優先度の高さからModifierと独立にパース出来るのでsubj/objとは別にNPレベルで共用してOK?

<Subj/Objの違いについて>
- baba eat lonely door は不成立
- baba eat door near wall は不成立
  * Pre/Post-modifierは動詞の左側 (Subj)のみで使用可能


<Notation>
- Noun = BABA, KEKE, etc.
- Verb:
   * be-Verb = IS
   * gen-Verb = HAS, MAKE
- Modifier:
   * PreM: Pre-Modifier = LONELY
   * PostM Post-Modifier = ON, NEAR, FACING
- Property: PUSH, SINK, etc.
- NOT
- AND


<Grammars>
Rule = Subj VP
VP = gen-Verb NP
VP = be-Verb NP
VP = be-Verb Comp

NP = Noun
NP = NOT NP
NP = NP AND NP

PreMP = PreM
PreMP = NOT PreMP
PostMP = PostM NP
PostMP = NOT PostMP
Subj = (PreMP)? NP (PostMP)?

Comp = Property
Comp = NOT Comp
Comp = NP AND Comp
Comp = Comp AND NP
Comp = Comp AND Comp


<優先度順> # NOT, ANDのルールは変化が無くなるまで　
- Noun&Property < NOT < AND < Pre-Modifier < Post-Modifier < Verb
- PreM, PreMPのところ、NPを取り込んでしまっても良いかも
- パース構造記述後にルールの簡略化を検討

NP = Noun
Comp = Property

NP = NOT NP
NP = NP AND NP

Comp = NOT Comp
Comp = NP AND Comp
Comp = Comp AND NP
Comp = Comp AND Comp

PreMP = PreM
PreMP = NOT PreMP
PostMP = PostM NP
PostMP = NOT PostMP

VP = gen-Verb NP
VP = be-Verb Comp
VP = be-Verb NP

Subj = PreMP NP PostMP
Subj = PreMP NP
Subj = NP PostMP
Rule = Subj VP




<parsing中の非文条件>
- 動詞が2つ以上
- PreM/PostMが

*/


#include <baba-is-auto/Rules/Rule.hpp>

namespace baba_is_auto
{

// Rule::Rule(TypeSequence types)
// {
//     // ParseTree(types);
//     objectTypes = types;
// }

Rule::Rule(ObjectType type1, ObjectType type2, ObjectType type3)
{
    objectTypes = { type1, type2, type3 };
}


// void Rule::ParseTree(TypeSequence types){
//     m_is_valid = true;

//     if (types.size() == 1){
// 	m_left.reset(nullptr);
// 	m_right.reset(nullptr);
// 	m_center = std::get<0>(types);

// 	// a span with the length == 1 must be a noun or a property.
// 	if (IsVerbType(m_center) ||  IsPropertyType(m_center)){
// 	    m_is_valid = false;
// 	}
// 	return;
//     }

//     // Center priority: verb > post-modifier > pre-modifier > AND > NOT > noun&property


//     auto itr = std::find_if(types.begin(), types.end(), IsVerbType);
//     if (itr == types.end()) {
// 	itr = std::find_if(types.begin(), types.end(), IsPostModifierType);
//     }
//     if (itr == types.end()) {
// 	itr = std::find_if(types.begin(), types.end(), IsPreModifierType);
//     }
//     if (itr == types.end()) {
// 	itr = std::find_if(types.begin(), types.end(), IsAND);
//     }
//     if (itr == types.end()) {
// 	itr = std::find_if(types.begin(), types.end(), IsNOT);
//     }


//     if (itr == types.end()) { // Noun or Property
// 	m_left.reset(nullptr);
// 	m_right.reset(nullptr);
// 	m_center = std::get<0>(types);
// 	if (types.size() != 1){
// 	    // Invalid: noun or property must be a single leaf.
// 	    m_is_valid = false;
// 	}
// 	return;
//     }


//     const int center_idx = std::distance(types.begin(), itr);
//     std::slice s1(0, center_idx, 1);
//     std::slice s2(center_idx + 1, types.size() - 1 - center_idx, 1);

//     if (center_idx == 0) {
// 	m_left.reset(nullptr);
//     } else {
// 	m_left.reset(Rule(types[s1]));
//     }

//     if (center_idx == types.size() - 1) {
// 	m_right.reset(nullptr);
//     } else {
// 	m_right.reset(Rule(types[s2]));
//     }

// }


bool Rule::IsValid() const {
    // bool left_valid = m_left == nullptr ? true : m_left.IsValid();
    // bool right_valid = m_right == nullptr ? true : m_right.IsValid();
    // return m_is_valid && left_valid && right_valid;
    return true;
}


bool Rule::operator==(const Rule& rhs) const
{
    return objectTypes == rhs.objectTypes;
}

ObjectType Rule::GetSubject() const
{
    // return ObjectType::BABA;
    // return objectTypes[0];
    return std::get<0>(objectTypes);
}
ObjectType Rule::GetOperator() const
{
    // return ObjectType::IS;
    // return objectTypes[1];
    return std::get<1>(objectTypes);
}
ObjectType Rule::GetPredicate() const
{
    // return ObjectType::YOU;
    // return objectTypes[2];
    return std::get<2>(objectTypes);
}




RuleNode::RuleNode(ObjectType center, 
		   std::shared_ptr<RuleNode> left,
		   std::shared_ptr<RuleNode> right)
{
    m_center = center;
    m_left = left;
    m_right = right;
}



bool RuleNode::operator==(const ObjectType& type) const
{
    return m_center == type;
}




}  // namespace baba_is_auto
