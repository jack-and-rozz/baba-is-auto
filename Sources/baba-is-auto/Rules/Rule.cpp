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

*/


#include <baba-is-auto/Rules/Rule.hpp>

namespace baba_is_auto
{

Rule::Rule(ObjectType type1, ObjectType type2, ObjectType type3)
{
    objectTypes = { type1, type2, type3 };
}


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


RuleNode::RuleNode(ObjectType top)
{
    m_top = top;
}


RuleNode::RuleNode(ObjectType top, RuleNode left)
{
    m_top = top;
    m_left = std::make_shared<RuleNode>(left);
}

RuleNode::RuleNode(ObjectType top, RuleNode left, RuleNode right)
{
    m_top = top;
    m_left = std::make_shared<RuleNode>(left);
    m_right = std::make_shared<RuleNode>(right);
}

bool RuleNode::operator==(const ObjectType& type) const
{
    return m_top == type;
}


bool RuleNode::HasTargetType(const ObjectType& type) const
{
    bool left_result = false;
    bool right_result = false;

    if (m_right.get() != nullptr){
	right_result = m_right->HasTargetType(type);
    }
    if (m_left.get() != nullptr && m_top != ObjectType::Rule){
	// check only whether the right node (=VP) contains type.
	left_result = m_left->HasTargetType(type);
    }
    if (m_right.get() == nullptr && m_left.get() == nullptr){
	// leaf nodes
	if (m_top == type){
	    return true;
	}
    }
    return left_result || right_result;
}

bool RuleNode::SatisfyCondition(const Object& obj,  const Map& map) const
{

    bool left_result = false;
    bool right_result = false;

    if (m_left.get() != nullptr){
	left_result = m_left->SatisfyCondition(obj, map);
    }
    if (m_right.get() != nullptr){
	right_result = m_right->SatisfyCondition(obj, map);
    }
    //      Rule
    //   Subj   VP
    //  NP    Verb NP
    // Baba   is keke

    // if (m_top == ObjectType::Rule){
    // 	// check subject.
    // 	return left_result;
    // }

    // (A and B) on (C and D) is PUSH

    // memo: NOTはboolのフラグにして毎回nodeの結果を返すときにフラグとXOR取る方が良い？
    // if (IsNounType(m_top)){
    // 	return m_top == obj.GetType();
    // } else if (IsGrammarType(m_top)){
    // 	if (m_top == ObjectType::Rule){
    // 	    return left_result;
    // 	else if (m_top == ObjectType::NP){
    // 	    ;;
    // 	} else if (m_top == ObjectType::PreMP){
    // 	    ;;
    // 	} else if (m_top == ObjectType::PostMP){ // left-> PostM, right->NP
    //        //       NP -> left || right
    //        //   NP (and) NP
    //        //  KEKE     BABA

    // 	   //      NP -> left ^ right 
    //        //   NOT   NP
    //        // NOT NOT  BABA

    // 	    //   POSTMP
    // 	    // PostM   NP
    //         // NEAR    WALL

    // 	    ;;
    // 	} else if (m_top == ObjectType::ON){
    // 	    baba and keke, obj=baba 
    // 	    if (left_result == true){
    // 		on_objs = m_right->GetObjects(); // C, D
    // 		obj.on(map, on_objs);
    // 	    }
    // 	} else if (m_top == ObjectType::LONELY){
    // 	    return obj.isAlone(map);
    // 	}
    // }

    //return left_result;
    return left_result && right_result;

    //return left_result && right_result;
}




}  // namespace baba_is_auto
