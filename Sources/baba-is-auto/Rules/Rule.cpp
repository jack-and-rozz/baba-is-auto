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
↑は旧バージョン。RuleManager.cppを参照
*/


#include <baba-is-auto/Rules/Rule.hpp>

#include <stdexcept>
#include <set> // std::multiset

namespace baba_is_auto
{

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

bool RuleNode::operator==(const RuleNode& node) const
{
    // First, compare the m_top member
    if (m_top != node.m_top) {
        return false;
    }

    // Compare the left children
    if (m_left && node.m_left) {
        // If both left children exist, compare them recursively
        if (!(*m_left == *node.m_left)) {
            return false;
        }
    } else if (m_left || node.m_left) {
        // If one exists and the other doesn't, they are not equal
        return false;
    }

    // Compare the right children
    if (m_right && node.m_right) {
        // If both right children exist, compare them recursively
        if (!(*m_right == *node.m_right)) {
            return false;
        }
    } else if (m_right || node.m_right) {
        // If one exists and the other doesn't, they are not equal
        return false;
    }

    // All checks passed, the nodes are equal
    return true;
}

// bool RuleNode::HasTargetType(const ObjectType& type) const
// {
//     bool left_result = false;
//     bool right_result = false;

//     if (m_right.get() != nullptr){
// 	right_result = m_right->HasTargetType(type);
//     }
//     if (m_left.get() != nullptr && m_top != ObjectType::Rule){
// 	// check only whether the right node (=VP) contains type.
// 	left_result = m_left->HasTargetType(type);
//     }
//     if (m_right.get() == nullptr && m_left.get() == nullptr){
// 	// leaf nodes
// 	if (m_top == type){
// 	    return true;
// 	}
//     }
//     return left_result || right_result;
// }



// const RuleNode& RuleNode::GetSubject() const 
// {
//     ;;
// }

// ObjectType RuleNode::GetVerb() const 
// {
//     ;;
// }

// const RuleNode& RuleNode::GetComplement() const
// {
//     ;;
// }

TypeSequence RuleNode::GetLeaves() const
{
    // Return all leaves.
    TypeSequence left_result;
    TypeSequence right_result;

    if ((m_left.get() == nullptr) && (m_right.get() == nullptr)){
	// leaf
	left_result.emplace_back(m_top);
    } else {
	// node
	if (m_left.get() != nullptr){
	    left_result = m_left->GetLeaves();
	}
	if (m_right.get() != nullptr){
	    right_result = m_right->GetLeaves();
	    left_result.insert(left_result.end(), 
			       right_result.begin(), 
			       right_result.end());
	}
    }
    return left_result;
}


bool RuleNode::ParseNOTtoBool() const 
{
    TypeSequence vec = GetLeaves();
    // Check if the number of NOT is even (true) or odd (false).
    if (!std::all_of(vec.begin(), vec.end(), [](ObjectType obj) {
        return obj == ObjectType::NOT;
    })) {
        throw std::runtime_error("This NOT node contains types other than ObjectType::NOT");
    }

    return vec.size() % 2 == 0;
}


TypeSequence RuleNode::ParseNPtoTypes() const 
{

    TypeSequence result;
    if (m_top == ObjectType::NP){
	if (m_left->m_top == ObjectType::NOT){
	    // NP = NOT NP

	    result = m_right->ParseNPtoTypes();
	    if (m_left->ParseNOTtoBool() == false){
		TypeSequence all_nouns = GetAllNouns();
		// a.erase(
		// 	std::remove_if(a.begin(), a.end(), [&b](int value) {
		// 					       return std::find(b.begin(), b.end(), value) != b.end();
		// 					   }),
		// 	a.end()
		// 	);
	    }

	} else if (m_left->m_top == ObjectType::NP){
	    // NP = NP AND NP
	    TypeSequence left_result;
	    TypeSequence right_result;

	    left_result = m_left->ParseNPtoTypes();
	    right_result = m_right->ParseNPtoTypes();

	    result.insert(result.end(), 
			  left_result.begin(), 
			  left_result.end());
	    result.insert(result.end(), 
			  right_result.begin(), 
			  right_result.end());

	} else {
	    // NP = Noun
	    //(todo)

	}


    }

    return result;
}


bool IsPreMPConditionSatisfied(const Object& obj,  const Map& map,
			       const ObjectType& pre_m) 
{
    return true; // (WIP)

    if (pre_m == ObjectType::LONELY){
	;;
    } else {
	throw std::runtime_error("This pre-modifier is not implemented");
    }

}

bool IsPostMPConditionSatisfied(const Object& obj,  const Map& map,
				const ObjectType& post_m,
				const TypeSequence& target_nouns)
{
    return true; // (WIP)

    if (post_m == ObjectType::ON){
	// PostMP = ON NP
	;;
    } else if (post_m == ObjectType::NEAR){
	// PostMP = NEAR NP
	;;
    } else if (post_m == ObjectType::FACING){
	// PostMP = FACING NP
	;;
    } else {
	throw std::runtime_error("This post-modifier is not implemented");
    }

}

bool RuleNode::IsSubjectConditionSatisfied(const Object& obj,  const Map& map) const
{
    // Check if the given Object satisfies the condition to be a subject of the rule.
    bool left_result = true;
    bool right_result = true;

    if (m_left.get() != nullptr){
    	left_result = m_left->IsSubjectConditionSatisfied(obj, map);
    }
    if (m_right.get() != nullptr){
    	right_result = m_right->IsSubjectConditionSatisfied(obj, map);
    }

    /*
      As NOT node can be a single NOT or a seuqnce of NOT, its boolean can be both true and false.
     */
    if (m_top == ObjectType::Rule){
	return left_result;
    } else if (m_top == ObjectType::VP){
	return true;
    } else if (m_top == ObjectType::Subj){
	return left_result && right_result;
    } else if (m_top == ObjectType::PreMP){
	return left_result && right_result;
    } else if (m_top == ObjectType::PostMP){
       	if (m_left->m_top == ObjectType::NOT){
	    // PostMP = NOT PostMP
	    return left_result == right_result;
	} else if (m_left->m_top == ObjectType::PostMP){
	    // PostMP = PostMP AND PostMP
	    return left_result && right_result;
	} else {
	    // PostMP = (ON, NEAR, FACING) NP
	    TypeSequence np_types = m_right->ParseNPtoTypes();
	    return IsPostMPConditionSatisfied(obj, map, m_left->m_top, np_types);

	    // if (m_left->m_top == ObjectType::ON){
	    // 	// PostMP = ON NP
	    // 	;;
	    // } else if (m_left->m_top == ObjectType::NEAR){
	    // 	// PostMP = NEAR NP
	    // 	;;
	    // } else if (m_left->m_top == ObjectType::FACING){
	    // 	// PostMP = FACING NP
	    // 	;;
	    // }
	}

    } else if (m_top == ObjectType::PreM){
	if (m_left->m_top == ObjectType::NOT){
	    // PreM = NOT PreM
	    return left_result == right_result;
	} else {
	    // PreM = LONELY
	    return IsPreMPConditionSatisfied(obj, map, m_left->m_top);
	    ;;
	}
    } else if (m_top == ObjectType::NP){
	if (m_left->m_top == ObjectType::NOT){
	    // NP = NOT NP
	    return left_result == right_result;
	} else if (m_left->m_top == ObjectType::NP){
	    // NP = NP AND NP
	    return left_result || right_result;
	} else if (m_left->m_top == obj.GetType()){
	    // NP = Noun
	    return true;
	} else {
	    return false;
	}
    } else if (m_top == ObjectType::NOT){
	// e.g.)
	//      not
	//   not    not
	// not not
	if (m_left.get() == nullptr){
	    // NOT (leaf)
	    return false;
	} else {
	    // NOT (x times) + NOT
	    return !left_result;
	}
    }
    return false;


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

    // 	    //   PostMP
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
    // return left_result && right_result;

    //return left_result && right_result;
}




}  // namespace baba_is_auto
