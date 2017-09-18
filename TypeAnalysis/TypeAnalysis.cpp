//////////////////////////////////////////////////////////////////////////
// TypeAnalysis.cpp - Type Analysis Library								//
// Ver 1.0																//
// Application: Contains Functions and Test Stub for TypeAnalysis.h		//
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015		//
// Author:      Syed Yaser Ahmed Syed									//
//              sysyed@syr.edu											//
//////////////////////////////////////////////////////////////////////////


#include "TypeAnalysis.h"
#include <Windows.h>

//Constructor that initializes ASTref_,scopeStack_,toker_ by using getInstance of Repository
TypeAnalysis::TypeAnalysis() :
	ASTref_(Repository::getInstance()->AST()),
	scopeStack_(Repository::getInstance()->scopeStack()),
	toker_(*(Repository::getInstance()->Toker()))
{}
 //checking against array of strings, to show what to include in type table
 bool TypeAnalysis::doDisplay(ASTNode* pNode)
{
	static std::string toDisplay[] = {
		"function", "lambda", "class", "struct", "enum", "alias", "typedef","namespace"
	};
	for (std::string type : toDisplay)
	{
		if (pNode->type_ == type)
			return true;
	}
	return false;
}
 //function for recursively searching depth first through Abstract syntax tree
 void TypeAnalysis::DFS(ASTNode* pNode)
{
	static std::string path = "";
	static std::string nspace = "";
	if (pNode->path_ != path)
		path = pNode->path_;
	if (pNode->type_ == "namespace")
		nspace = pNode->name_;
	if (doDisplay(pNode))
	{
		if (pNode->type_ != "control" || pNode->name_ != "Global Namespace" || pNode->name_ != "main")
		{
			tt->addType(path, nspace, pNode->name_);
			tt->loadAnimation();
		}
	}
	for (auto pChild : pNode->children_)
		DFS(pChild);
}
 //function for returning namespace string
 std::string TypeAnalysis::setnSpace(std::string ns)
{
	return ns;
}
 //makes use of ASTRef and DFS for searching and adding values to type table
 std::unordered_map<std::string, _pair> TypeAnalysis::doTypeAnal()
{
	ASTNode* pRoot = ASTref_.root();
	std::cout << "\n Analyzing types Please Wait...";
	DFS(pRoot);
	std::cout <<"\n\n Type Analysis Completed!\n";
	//tt->displayMap();
	return tt->returnMap();
}

#ifdef DEBUG_TYPEA
 int main()
 {
	 TypeAnalysis TA1;
	 std::unordered_map<std::string, _pair> newtable = TA1.doTypeAnal();
 }
#endif // DEBUG_TYPEA

