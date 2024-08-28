// Copyright (c) 2020 Chris Ohk

// I am making my contributions/submissions to this project solely in our
// personal capacity and am not conveying any rights to any intellectual
// property of any third parties.

#include <baba-is-auto/Rules/RuleManager.hpp>

#include <algorithm>
#include <tuple>

namespace baba_is_auto
{

void DbgPrint(std::string title, TypeSequence types)
{
    std::cout << title << std::endl;
    for (ObjectType& t: types)
	{
	    std::cout << static_cast<int>(t) << " ";
	}
    std::cout << std::endl;
}

RuleManager::RuleManager()
{
    /*
       <Grammars>
       NOT = NOT NOT
       NP = Noun
       Comp = Property

       NP = NOT NP
       NP = NP AND NP

       Comp = NOT Comp
       Comp = NP AND Comp
       Comp = Comp AND NP
       Comp = Comp AND Comp

       PreM = Pre-Modifier (LONELY)
       PreM = NOT PreM
       PreMP = PreM NP
       PostM = Post-Modifier (ON, NEAR, FACING)
       PostMP = PostM NP
       PostMP = NOT PostMP

       VP = gen-Verb NP
       VP = IS Comp
       VP = IS NP

       Subj = PreMP PostMP
       Subj = NP PostMP
       Subj = PreMP
       Subj = NP

       Rule = Subj VP

       <memo>
       - 判定は Noun&Property < NOT < AND < Pre-Modifier <= Post-Modifier < Verb
       - パースは逆
    */
    // NOT = NOT NOT
    {
	TypeSequence src{ObjectType::NOT, ObjectType::NOT};
	ObjectType tgt = ObjectType::NOT;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // NP = Noun
    for (auto& x: GetAllNouns()){
	TypeSequence src{x};
	ObjectType tgt = ObjectType::NP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // Comp = Property
    for (auto& x: GetAllProperties()){
	TypeSequence src{x};
	ObjectType tgt = ObjectType::Complement;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // NP = NOT NP
    {
	TypeSequence src{ObjectType::NOT, ObjectType::NP};
	ObjectType tgt = ObjectType::NP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // NP = NP AND NP
    {
	TypeSequence src{ObjectType::NP, ObjectType::AND, ObjectType::NP};
	ObjectType tgt = ObjectType::NP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // Comp = NOT Comp
    {
	TypeSequence src{ObjectType::NOT, ObjectType::Complement};
	ObjectType tgt = ObjectType::Complement;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // Comp = NP AND Comp
    {
	TypeSequence src{ObjectType::NP, ObjectType::AND, ObjectType::Complement};
	ObjectType tgt = ObjectType::Complement;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // Comp = Comp AND NP
    {
	TypeSequence src{ObjectType::Complement, ObjectType::AND, ObjectType::NP};
	ObjectType tgt = ObjectType::Complement;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // Comp = Comp AND Comp
    {
	TypeSequence src{ObjectType::Complement, ObjectType::AND, ObjectType::Complement};
	ObjectType tgt = ObjectType::Complement;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // PreM = Pre-Modifier
    for (auto& x: GetAllPreModifiers()){
	TypeSequence src{x};
	ObjectType tgt = ObjectType::PreM;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // PreM = NOT PreM
    {
	TypeSequence src{ObjectType::NOT, ObjectType::PreM};
	ObjectType tgt = ObjectType::PreM;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // PreMP = PreM NP
    {
	TypeSequence src{ObjectType::PreM, ObjectType::NP};
	ObjectType tgt = ObjectType::PreMP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // PostM = Post-Modifier (この階層要るか？)
    for (auto& x: GetAllPostModifiers()){
	TypeSequence src{x};
	ObjectType tgt = ObjectType::PostM;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // // PostM = NOT PostM
    // {
    // 	TypeSequence src{ObjectType::NOT, ObjectType::PostM};
    // 	ObjectType tgt = ObjectType::PostM;
    // 	m_grammars.emplace_back(std::make_tuple(src, tgt));
    // }

    // PostMP = PostM NP
    {
	TypeSequence src{ObjectType::PostM, ObjectType::NP};
	ObjectType tgt = ObjectType::PostMP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // PostMP = NOT PostMP
    {
	TypeSequence src{ObjectType::NOT, ObjectType::PostMP};
	ObjectType tgt = ObjectType::PostMP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }


    // VP = gen-Verb NP
    for (auto& x: GetAllGenVerbs()){
	TypeSequence src{x, ObjectType::NP};
	ObjectType tgt = ObjectType::VP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // VP = IS Comp
    {
	TypeSequence src{ObjectType::IS, ObjectType::Complement};
	ObjectType tgt = ObjectType::VP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // VP = IS NP
    {
	TypeSequence src{ObjectType::IS, ObjectType::NP};
	ObjectType tgt = ObjectType::VP;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // Subj = PreMP PostMP
    {
	TypeSequence src{ObjectType::PreMP, ObjectType::PostMP};
	ObjectType tgt = ObjectType::Subj;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // Subj = NP PostMP
    {
	TypeSequence src{ObjectType::NP, ObjectType::PostMP};
	ObjectType tgt = ObjectType::Subj;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // Subj = PreMP
    {
	TypeSequence src{ObjectType::PreMP};
	ObjectType tgt = ObjectType::Subj;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }
    // Subj = NP
    {
	TypeSequence src{ObjectType::NP};
	ObjectType tgt = ObjectType::Subj;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // Rule = Subj VP
    {
	TypeSequence src{ObjectType::Subj, ObjectType::VP};
	ObjectType tgt = ObjectType::Rule;
	m_grammars.emplace_back(std::make_tuple(src, tgt));
    }

    // <debug>
    // std::cout << "Grammars" << std::endl;
    // for (auto& g: m_grammars){
    // 	TypeSequence v = std::get<0>(g);
    // 	ObjectType tgt = std::get<1>(g);
    // 	std::cout << "[ ";
    // 	for (auto& src: v){
    // 	    std::cout << static_cast<int>(src) << " ";
    // 	}
    // 	std::cout << "] ->" << static_cast<int>(tgt) ;
    // 	std::cout << std::endl;
    // }

}

void PrintNodeList(int i, std::vector<RuleNode>& nodes){
    std::cout << "PrintNodeList: " << i <<  std::endl;
    for (auto& node: nodes){
	std::cout << static_cast<int>(node.m_top) << " ";
    }
    std::cout << std::endl;
}

void PrintGrammar(TypeSequence src, ObjectType tgt)
{
    for (auto& s: src){
	std::cout << static_cast<int>(s) << " ";
    }
    std::cout << " -> ";
    std::cout << static_cast<int>(tgt) << std::endl;
}

std::vector<RuleNode> UpdateNodes(std::vector<RuleNode> nodes, 
				  RuleNode new_node,
				  std::size_t cut_start_idx, 
				  std::size_t cut_end_idx)
{
    /*
      e.g., 
      nodes = [0,1,2,3,4,5]
      new_node = [2, 3]
      cut_start_idx = 2
      cut_end_idx = 4
      new_nodes = [0,1] + [new_node] + [4,5] 
    */

    std::vector<RuleNode> new_nodes;
    if (cut_start_idx > 0){
	std::copy(nodes.begin(), nodes.begin() + cut_start_idx,
		  std::back_inserter(new_nodes));
    }
    new_nodes.emplace_back(new_node);
    if (nodes.begin() + cut_end_idx < nodes.end()){
	std::copy(nodes.begin() + cut_end_idx, nodes.end(),
		  std::back_inserter(new_nodes));
    }
    return new_nodes;

}

std::optional<RuleNode> RuleManager::BuildRuleTree(TypeSequence seq){
    /* e.g., 
       not lonely baba and keke near wall is not push and stop
       -> not PreM NP and NP PostM NP is not Comp and Comp
       -> not PreM NP PostM NP is not Comp and Comp
       -> not PreM NP PostM NP is Comp
       -> PreMP PostMP is Comp
       -> PreMP PostMP VP
       -> Subj VP
       -> Rule
     */

    std::vector<RuleNode> nodes;
    std::vector<RuleNode> sliced;
    // Put all words (leaves) to a node list.
    for (auto& t: seq){
    	RuleNode node = RuleNode(t);
    	nodes.emplace_back(node);
    }

    // Apply a grammar from left to right and update the node list.
    std::cout << "<Node parsing>" << std::endl;
    for (auto& g: m_grammars){
    	TypeSequence src = std::get<0>(g);
    	ObjectType tgt = std::get<1>(g);
	std::size_t i = 0;
	std::size_t left_idx = 0;
	std::size_t right_idx = 0;

	//while (i <= nodes.size() - src.size()){
	while (nodes.begin() + i + src.size() <= nodes.end()){
	    // Slice src from the current sequences of odes.
	    sliced = std::vector<RuleNode>(nodes.begin() + i, nodes.begin() + i + src.size());

	    // vector::operator== は内部比較で定義済みのRuleNode::operator==を使ってくれない？
	    //if (sliced == src){
	    if (std::equal(sliced.cbegin(), sliced.cend(), src.cbegin())){
		std::cout << "=============" << std::endl;
		std::cout << "<Grammar>" << std::endl;
		PrintGrammar(src, tgt);
		std::cout << "<Current Nodes>" << std::endl;
		PrintNodeList(i, nodes);
		if (src.size() == 1){
		    left_idx = 0;
		} else if (src.size() == 2){
		    left_idx = 0;
		    right_idx = 1;
		} else if (src.size() == 3){
		    // sliced.at(1) is 'AND' (OR does not exist, so ignored).
		    left_idx = 0;
		    right_idx = 2;
		} else if (src.size() >= 4) {
		    throw "Exception : the src length of a grammar must be 1-3.\n";
		}
		if (src.size() == 1){
		    RuleNode new_node = RuleNode(tgt, sliced.at(left_idx));
		} else {
		    RuleNode new_node = RuleNode(tgt, sliced.at(left_idx), sliced.at(right_idx));
		}
		nodes = UpdateNodes(nodes, new_node, i, i+src.size());
		std::cout << "<Next Nodes>" << std::endl; 
		PrintNodeList(i, nodes);
	    } else {
		// Go right if the grammar does not match to the current span.
		i++;
	    }
	}
    }
    // Finally, ObjectType::Rule only remains on the top of a tree if it is valid.
    if (nodes.size() == 1 && nodes.at(0) == ObjectType::Rule){
	return nodes[0];
    }
    return std::nullopt;

    // TypeSequence nouns = GetAllNouns();
    // std::cout << "All nouns" << std::endl;
    // for (auto& n: nouns)
    // 	{
    // 	    std::cout << static_cast<int>(n) << " " ;
    // 	}
    // std::cout << std::endl;


    //return;
}


//void RuleManager::AddRule(const Rule& rule)
void RuleManager::AddRule(const RuleNode& rule)
{
    m_rules.emplace_back(rule);
}

//void RuleManager::RemoveRule(const Rule& rule)
void RuleManager::RemoveRule(const RuleNode& rule)
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

std::vector<RuleNode> RuleManager::GetAllRules() const
{
    return m_rules;
}

std::vector<RuleNode> RuleManager::GetRules(ObjectType type) const
{
    std::vector<RuleNode> ret;

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


bool RuleManager::HasType(const Object& obj, const Map& map, ObjectType tgtType) const {
    auto objType = obj.GetType();
    objType = IsIconType(objType) ? ConvertIconToText(objType) : ObjectType::TEXT;

    // The three rules always implicitly exist unless denied.
    // (TEXT IS PUSH, LEVEL IS STOP, CURSOR IS SELECT)
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
	// rule.HasTargetType(tgtType);
	// rule.SatisfyCondition(obj, map);

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
}


void RuleManager::ParseRule(Map& map, std::size_t x, std::size_t y, RuleDirection direction)
{

    /* Notes (letra418):
       - Currently only one text block is considered when overlapping.
    */

    // 1. Find the longest sequence of text blocks from (x, y).
    TypeSequence longest_seq;
    if (direction == RuleDirection::HORIZONTAL){
	const std::size_t width = map.GetWidth();
    	for (std::size_t xx=x; xx<width; ++xx){
	    if (map.At(xx, y).HasTextType()){
    	        ObjectType type = map.At(xx, y).GetTextObjects()[0].GetType();
    	    	longest_seq.emplace_back(type);
    	    } else {
    	    	break;
    	    }
    	}
    } else if (direction == RuleDirection::VERTICAL){
    	const std::size_t height = map.GetHeight();
    	for (std::size_t yy=y; yy<height; ++yy){
    	    if (map.At(x, yy).HasTextType()){
    	        ObjectType type = map.At(x, yy).GetTextObjects()[0].GetType();
    		longest_seq.emplace_back(type);
    	    } else {
    		break;
    	    }
    	}
    }
    // ==== DEBUG ====
    // if (longest_seq.size() >=3){
    // 	std::cout << "longest_seq";
    // 	for (auto& x: longest_seq)
    // 	    {
    // 		std::cout << static_cast<int>(x) << " ";
    // 	    }
    // 	std::cout << std::endl;
    // }

    // 2. Starting from the longest sequence, continue cutting off the last text block and parsing a current sequence till finding a valid rule.
    TypeSequence seq = longest_seq;
    while (true) {
    	if (seq.size() < 3) break;
    	//Rule rule = Rule(seq);
	std::cout << "(x, y, dir) = [" << x << ", " << y << ", " << static_cast<int>(direction) << "]" << std::endl;
    	//Rule rule = Rule(seq[0], seq[1], seq[2]);
	//BuildRuleTree(seq); // debug for new parsing system (does not affect the game)
	std::optional<RuleNode> rule = BuildRuleTree(seq);

    	if (rule.has_value() && (direction == RuleDirection::HORIZONTAL)){
    	    for (std::size_t xx=x; xx<x+seq.size(); ++xx){
    		map.At(xx, y).isRule = true;
    	    }
    	    AddRule(*rule);
    	    break;
    	}
    	if (rule.has_value() && (direction == RuleDirection::VERTICAL)){
    	    for (std::size_t yy=y; yy<y+seq.size(); ++yy){
    		map.At(x, yy).isRule = true;
    	    }
    	    AddRule(*rule);
    	    break;
    	}

    	seq = TypeSequence(seq.begin(), seq.end()-1);
    }

    return;
}

TypeSequence RuleManager::GetAllNouns()
{
    TypeSequence s;
    int begin = static_cast<int>(ObjectType::NOUN_TYPE) + 1;
    int end = static_cast<int>(ObjectType::OP_TYPE);
    for (int i=begin; i<end; ++i){
	s.emplace_back(static_cast<ObjectType>(i));
    }
    return s;
}
TypeSequence RuleManager::GetAllProperties()
{
    TypeSequence s;
    int begin = static_cast<int>(ObjectType::PROPERTY_TYPE) + 1;
    int end = static_cast<int>(ObjectType::ICON_TYPE);
    for (int i=begin; i<end; ++i){
	s.emplace_back(static_cast<ObjectType>(i));
    }
    return s;
}

TypeSequence RuleManager::GetAllGenVerbs()
{
    TypeSequence s{ObjectType::HAS, ObjectType::MAKE};
    return s;
}

TypeSequence RuleManager::GetAllPreModifiers()
{
    TypeSequence s{ObjectType::LONELY};
    return s;
}
TypeSequence RuleManager::GetAllPostModifiers()
{
    TypeSequence s{ObjectType::ON, ObjectType::NEAR, ObjectType::FACING};
    return s;
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
