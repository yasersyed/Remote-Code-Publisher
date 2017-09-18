/////////////////////////////////////////////////////////////////////
//  AbstrSynTree.cpp - Represents an Abstract Syntax Tree          //
//  ver 1.3                                                        //
//  Language:      Visual C++ 2015                                 //
//  Platform:      Dell XPS 8900, Windows 10                       //
//  Application:   Used to support parsing source code             //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include "AbstrSynTree.h"
#include "../Utilities/Utilities.h"

using namespace CodeAnalysis;

//----< default initialization for ASTNodes >------------------------

ASTNode::ASTNode()
	: type_("anonymous"), name_("none"), startLineCount_(0), endLineCount_(0), complexity_(0) {}

//----< initialization accepting type and name >---------------------

ASTNode::ASTNode(const Type& type, const Name& name)
	: type_(type), name_(name), startLineCount_(0), endLineCount_(0), complexity_(0) {}

//----< destructor deletes children, statements, declarations >------

ASTNode::~ASTNode()
{
	for (auto pNode : children_)
		delete pNode;
	for (auto pNode : statements_)
		delete pNode;
	for (auto node : decl_)
	{
		delete node.pTc;
	}
}
//----< returns string with ASTNode description >--------------------

std::string ASTNode::show(bool details)
{
	std::ostringstream temp;
	temp << "(";
	temp << type_ << ", ";
	if (name_ != "none")
		temp << name_ << ", ";
	if (type_ == "namespace" || type_ == "class" || type_ == "interface" || type_ == "struct" || type_ == "function")
		temp << package_ << ", ";
	if (details)
	{
		temp << "num children: " << children_.size() << ", ";
		temp << "num statements: " << statements_.size() << ", ";
	}
	temp << "line: " << startLineCount_ << ", ";
	temp << "size: " << endLineCount_ - startLineCount_ + 1 << ", ";
	temp << "cplx: " << complexity_;
	//temp << "path: " << path_;
	temp << ")";
	return temp.str();
}
//----< create instance and initialize first element >---------------
/*
*  - Every AST holds an ASTNode representing the global namespace
*  - Its children will be namespaces, classes, and global data
*/
AbstrSynTree::AbstrSynTree(ScopeStack<ASTNode*>& stack) : stack_(stack)
{
	pGlobalNamespace_ = new ASTNode("namespace", "Global Namespace");
	pGlobalNamespace_->startLineCount_ = 1;
	pGlobalNamespace_->endLineCount_ = 1;
	stack_.push(pGlobalNamespace_);
}
//----< destroy instance and all it's ASTNodes >---------------------
/*
*  AST destructor only has to delete its root.  The root will then
*  delete its children, causing the children to delete their children,
*  yada, yada, yada
*/
AbstrSynTree::~AbstrSynTree()
{
	delete pGlobalNamespace_;
}
//
//Move Constructor for Abstract Syntax Tree
AbstrSynTree::AbstrSynTree(AbstrSynTree&& o)
	:pGlobalNamespace_(o.pGlobalNamespace_),
	stack_(o.stack_),
	typeMap_(o.typeMap_)
{
	//Code for move constructor
	o.stack_.clear();
	o.pGlobalNamespace_ = new ASTNode();
	o.typeMap_.clear();
}

//Move Assignment Operator for Abstract Syntax Tree
AbstrSynTree& AbstrSynTree::operator=(AbstrSynTree&& old)
{
	if (this != &old) {
		this->pGlobalNamespace_ = std::move(old.pGlobalNamespace_);
		this->stack_ = std::move(old.stack_);
		this->typeMap_ = std::move(old.typeMap_);
		old.stack_.clear();
		old.pGlobalNamespace_ = new ASTNode();
		old.typeMap_.clear();
	}
	return *this;
}

//----< return or accept pointer to global namespace >---------------

ASTNode*& AbstrSynTree::root()
{
	return pGlobalNamespace_;
}
//----< return AST typemap >-----------------------------------------
/*
*  Type map holds ClassName keys linked to each class's ASTNode
*/
AbstrSynTree::TypeMap& AbstrSynTree::typeMap()
{
	return typeMap_;
}
//----< add ASTNode ptr to stack top element's children and push >---
/*
* - Add new scope to ScopeStack after linking to its parent scope
* - If type is a class or struct, add to typeMap
*/
void AbstrSynTree::add(ASTNode* pNode)
{
	stack_.top()->children_.push_back(pNode);  // add as child of stack top
	stack_.push(pNode);                        // push onto stack
	if (pNode->type_ == "class" || pNode->type_ == "struct" || pNode->type_ == "interface")
		typeMap_[pNode->name_] = pNode;
}
//----< pop stack's top element >------------------------------------
/*
*  - called at end of scope
*  - pop'd ASTNode is still linked to AST
*/
ASTNode* AbstrSynTree::pop()
{
	return stack_.pop();
}
//
ASTNode * CodeAnalysis::AbstrSynTree::findRecursively(const ClassName & type, ASTNode * node) {
	if (node->name_ == type) return node;
	for (auto n : node->children_) {
		if (n->name_ == type) return n;
	}
	for (auto n : node->children_)
		return findRecursively(type, n);
	return nullptr;
}
//----< find a type node using typeMap >-----------------------------
/*
*  Retrieve ASTNode of class if it exists
*/
ASTNode* AbstrSynTree::find(const ClassName& className)
{
	auto iter = typeMap_.find(className);
	if (iter != typeMap_.end())
	{
		return iter->second;
	}
	return findRecursively(className, pGlobalNamespace_);
}

//----< Test Stub >--------------------------------------------------
/*
*  Demonstrates that AbstrSynTree can build a simple Abstract
*  Syntax Tree and display its contents.
*/
#ifdef TEST_ABSTRSYNTREE

#include <functional>
#include <iostream>

using Utils = Utilities::StringHelper;

int main()
{
	Utils::Title("Testing AbstrSynTree and ASTNode");

	ScopeStack<ASTNode*> stack;
	AbstrSynTree ast(stack);
	ASTNode* pX = new ASTNode("class", "X");
	ast.add(pX);                                        // add X scope
	Utils::title("testing AbstrSynTree::find");

	ASTNode* pFind = ast.find("X");
	if (pFind != nullptr)
		std::cout << "\n  " << pFind->show();
	else
		std::cout << "\n  could not find ASTNode for class X";

	std::cout << "\n\n";
}

#endif
