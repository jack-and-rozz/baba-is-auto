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



baba is keke 
-> NP be-verb NP

baba is keke and push
-> NP be-verb NP and Prop

<Rule patterns>
Rule = Subj VP
VP = gen-Verb Obj
VP = be-Verb Comp

NP = Noun
NP = NOT NP
NP = NP AND NP

# Subj = (NOT* PreM)? NP (NOT* PostM NP)?

PreMP = PreM
PreMP = NOT PreMP
PostMP = PostM NP
PostMP = NOT PostMP
Subj = (PreMP)? NP (PostMP)?

Obj = NP
Comp = Property
Comp = NP
Comp = NOT Comp
Comp = Comp AND Comp


<優先度順> # NOT, ANDのルールは変化が無くなるまで　
NP = Noun
Comp = Property
PreMP = PreM

NP = NOT NP
NP = NP AND NP

Comp = NOT Comp
Comp = Comp AND Comp


PostMP = PostM NP
Subj = (PreMP)? NP (PostMP)?




<parsing中の非文条件>
- 動詞が2つ以上
- PreM/PostMが

*/


#include <baba-is-auto/Rules/Rule.hpp>

namespace baba_is_auto
{
Rule::Rule(TypeSequence types)
{
    ParseTree(types);
}



void Rule::ParseTree(TypeSequence types){
    m_is_valid = true;

    if (types.size() == 1){
	m_left.reset(nullptr);
	m_right.reset(nullptr);
	m_center = std::get<0>(types);

	// a span with the length == 1 must be a noun or a property.
	if (IsVerbType(m_center) ||  IsPropertyType(m_center)){
	    m_is_valid = false;
	}
	return;
    }

    // Center priority: verb > post-modifier > pre-modifier > AND > NOT > noun&property


    auto itr = std::find_if(types.begin(), types.end(), IsVerbType);
    if (itr == types.end()) {
	itr = std::find_if(types.begin(), types.end(), IsPostModifierType);
    }
    if (itr == types.end()) {
	itr = std::find_if(types.begin(), types.end(), IsPreModifierType);
    }
    if (itr == types.end()) {
	itr = std::find_if(types.begin(), types.end(), IsAND);
    }
    if (itr == types.end()) {
	itr = std::find_if(types.begin(), types.end(), IsNOT);
    }


    if (itr == types.end()) { // Noun or Property
	m_left.reset(nullptr);
	m_right.reset(nullptr);
	m_center = std::get<0>(types);
	if (types.size() != 1){
	    // Invalid: noun or property must be a single leaf.
	    m_is_valid = false;
	}
	return;
    }


    const int center_idx = std::distance(types.begin(), itr);
    std::slice s1(0, center_idx, 1);
    std::slice s2(center_idx + 1, types.size() - 1 - center_idx, 1);

    if (center_idx == 0) {
	m_left.reset(nullptr);
    } else {
	m_left.reset(Rule(types[s1]));
    }

    if (center_idx == types.size() - 1) {
	m_right.reset(nullptr);
    } else {
	m_right.reset(Rule(types[s2]));
    }

}
    // if (itr == types.end()) {
    // 	/* Invalid if:
    // 	   - there is neither a verb nor an operator in a given span, but more than one nouns still remain in it.
    // 	*/
    // 	m_left.reset(nullptr);
    // 	m_right.reset(nullptr);
    // 	m_center = std::get<0>(types);
    // 	m_is_valid = false;
    // 	return;
    // } else {
    // 	// when a verb or an operator is found in the span.

    // 	const int center_idx = std::distance(types.begin(), itr);
    // 	std::slice s1(0, center_idx, 1);
    // 	std::slice s2(center_idx + 1, types.size() - 1 - center_idx, 1);

    // 	if (center_idx == 0) {
    // 	    m_left.reset(nullptr);
    // 	} else {
    // 	    m_left.reset(Rule(types[s1]));
    // 	}

    // 	if (center_idx == types.size() - 1) {
    // 	    m_right.reset(nullptr);
    // 	} else {
    // 	    m_right.reset(Rule(types[s2]));
    // 	}

    // 	/* Invalid if:
    // 	   <1> a verb exists in the left or right span (only one verb can exist in a sentence).
    // 	   <2-1> no text in the left/right span of a verb.
    // 	   <2-2> a text after IS is neither a noun, a property, nor NOT.
    // 	   <2-3> a text after other verbs is neither a noun nor NOT.

    // 	   <3-1> no text in the right span of an op.

    // 	   <3-2> no text in the left span of AND.
    // 	   <3-3> a text after an op is neither a noun nor NOT.

    // 	   // memo:
    // 	   - baba not make keke は機能しない
    // 	      * notはnoun, property, adjectiveの直前のみ
    // 	      * not baba and 
    // 	 */
    // 	// <1>
    // 	ObjectType center = types[center_idx];
    // 	TypeSequence left = types[s1];
    // 	TypeSequence right = types[s2];
    // 	ObjectType nextword;
    // 	ObjectType prevword;

    // 	itr = std::find_if(left.begin(), left.end(), IsVerbType);
    // 	if (itr != left.end()){ m_is_valid = false; }
    // 	itr = std::find_if(right.begin(), right.end(), IsVerbType);
    // 	if (itr != right.end()){ m_is_valid = false; }

    // 	if (IsVerbType(center)){
    // 	    // <2-1>
    // 	    if (left.size() == 0){ m_is_valid = false; }
    // 	    if (right.size() == 0){ m_is_valid = false; }

    // 	    // <2-2>, <2-3>
    // 	    nextword = std::get<0>(right);
    // 	    if (center == ObjectType::IS){
    // 		if (!IsNounType(nextword) || !IsPropertyType(nextword) || nextword != ObjectType::NOT){
    // 		    m_is_valid = false;
    // 		}
    // 	    } else {
    // 		if (!IsNounType(nextword) || nextword != ObjectType::NOT){
    // 		    m_is_valid = false;
    // 		}
    // 	    }

    // 	    // <2-3>
    // 	    if (center != ObjectType::IS){
    // 		itr = std::find_if(right.begin(), right.end(), IsPropertyType);
    // 		if (itr != right.end()){ m_is_valid = false; }
    // 	    }

    // 	    <2-4>
    // 	    todo: near, on, facing, lonely cannot be in the right span of a verb.
    // 	}
    // 	if (IsOpType(center)){
    // 	    // <3-1>
    // 	    if (right.size() == 0){ m_is_valid = false; }

    // 	    if (center == ObjectType::AND){
    // 		// <3-2>
    // 		if (left.size() == 0){ m_is_valid = false; }
    // 	    } else if (center == ObjectType::NOT){
    // 		// <3-3>
    // 	    } else {
    // 		if (!IsNounType(std::get<0>(right)) && (std::get<0>(right) != ObjectType::NOT)){ m_is_valid = false; }
    // 	    }
    // 	}

    // }
    // return;
//}





// void Rule::ParseTree(TypeSequence types){
//     m_is_valid = true;

//     if (types.size() == 1){
// 	m_left.reset(nullptr);
// 	m_right.reset(nullptr);
// 	m_center = std::get<0>(types);
// 	// a span with the length == 1 must be a noun or a property.
// 	if (IsVerbType(m_center) ||  IsOpType(m_center)){
// 	    m_is_valid = false;
// 	}
// 	return;
//     }

//     // Center priority: verb > post-modifier > pre-modifier > AND > NOT
//     auto itr = std::find_if(types.begin(), types.end(), IsVerbType);
//     if (itr == types.end()) {
// 	itr = std::find_if(types.begin(), types.end(), IsModifierType);
//     }

//     if (itr == types.end()) {
// 	itr = std::find_if(types.begin(), types.end(), IsAND);
//     }
//     if (itr == types.end()) {
// 	itr = std::find_if(types.begin(), types.end(), IsNOT);
//     }



//     if (itr == types.end()) {
// 	/* Invalid if:
// 	   - there is neither a verb nor an operator in a given span, but more than one nouns still remain in it.
// 	*/
// 	m_left.reset(nullptr);
// 	m_right.reset(nullptr);
// 	m_center = std::get<0>(types);
// 	m_is_valid = false;
// 	return;
//     } else {
// 	// when a verb or an operator is found in the span.

// 	const int center_idx = std::distance(types.begin(), itr);
// 	std::slice s1(0, center_idx, 1);
// 	std::slice s2(center_idx + 1, types.size() - 1 - center_idx, 1);

// 	if (center_idx == 0) {
// 	    m_left.reset(nullptr);
// 	} else {
// 	    m_left.reset(Rule(types[s1]));
// 	}

// 	if (center_idx == types.size() - 1) {
// 	    m_right.reset(nullptr);
// 	} else {
// 	    m_right.reset(Rule(types[s2]));
// 	}

// 	/* Invalid if:
// 	   <1> a verb exists in the left or right span (only one verb can exist in a sentence).
// 	   <2-1> no text in the left/right span of a verb.
// 	   <2-2> a text after IS is neither a noun, a property, nor NOT.
// 	   <2-3> a text after other verbs is neither a noun nor NOT.

// 	   <3-1> no text in the right span of an op.

// 	   <3-2> no text in the left span of AND.
// 	   <3-3> a text after an op is neither a noun nor NOT.

// 	   // memo:
// 	   - baba not make keke は機能しない
// 	      * notはnoun, property, adjectiveの直前のみ
// 	      * not baba and 
// 	 */
// 	// <1>
// 	ObjectType center = types[center_idx];
// 	TypeSequence left = types[s1];
// 	TypeSequence right = types[s2];
// 	ObjectType nextword;
// 	ObjectType prevword;

// 	itr = std::find_if(left.begin(), left.end(), IsVerbType);
// 	if (itr != left.end()){ m_is_valid = false; }
// 	itr = std::find_if(right.begin(), right.end(), IsVerbType);
// 	if (itr != right.end()){ m_is_valid = false; }

// 	if (IsVerbType(center)){
// 	    // <2-1>
// 	    if (left.size() == 0){ m_is_valid = false; }
// 	    if (right.size() == 0){ m_is_valid = false; }

// 	    // <2-2>, <2-3>
// 	    nextword = std::get<0>(right);
// 	    if (center == ObjectType::IS){
// 		if (!IsNounType(nextword) || !IsPropertyType(nextword) || nextword != ObjectType::NOT){
// 		    m_is_valid = false;
// 		}
// 	    } else {
// 		if (!IsNounType(nextword) || nextword != ObjectType::NOT){
// 		    m_is_valid = false;
// 		}
// 	    }

// 	    // <2-3>
// 	    if (center != ObjectType::IS){
// 		itr = std::find_if(right.begin(), right.end(), IsPropertyType);
// 		if (itr != right.end()){ m_is_valid = false; }
// 	    }

	    // <2-4>
	    // todo: near, on, facing, lonely cannot be in the right span of a verb.
// 	}
// 	if (IsOpType(center)){
// 	    // <3-1>
// 	    if (right.size() == 0){ m_is_valid = false; }

// 	    if (center == ObjectType::AND){
// 		// <3-2>
// 		if (left.size() == 0){ m_is_valid = false; }
// 	    } else if (center == ObjectType::NOT){
// 		// <3-3>
// 	    } else {
// 		if (!IsNounType(std::get<0>(right)) && (std::get<0>(right) != ObjectType::NOT)){ m_is_valid = false; }
// 	    }
// 	}

//     }
//     return;
// }

bool Rule::IsValid() const {
    bool left_valid = m_left == nullptr ? true : m_left.IsValid();
    bool right_valid = m_right == nullptr ? true : m_right.IsValid();
    return m_is_valid && left_valid && right_valid;
}


// bool Rule::operator==(const Rule& rhs) const
// {
//     return objectTypes == rhs.objectTypes;
// }

// ObjectType Rule::GetSubject() const
// {
//     return std::get<0>(objectTypes);
// }
// ObjectType Rule::GetOperator() const
// {
//     return std::get<1>(objectTypes);
// }
// ObjectType Rule::GetPredicate() const
// {
//     return std::get<2>(objectTypes);
// }



bool Rule::HasProperty() const {
}

bool Rule::HasOperator() const {
}

bool Rule::HasNoun() const {
}

// std::vector<T> vec(n - m + 1);
//     std::copy(v.begin() + m, v.begin() + n + 1, vec.begin());



}  // namespace baba_is_auto
