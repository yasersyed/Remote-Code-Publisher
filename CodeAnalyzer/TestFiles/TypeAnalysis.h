#pragma once

//////////////////////////////////////////////////////////////////////
// TypeAnalysis.h									 				//
//																	//
// Package for Analysing types from list of files					//
// Ver 1.0															//
// Application: can store a key and a pair of string values			//
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015	//
// Author:      Syed Yaser Ahmed Syed								//
//              sysyed@syr.edu										//
//////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This header contains class for collecting type information from files and storing them in ASTNode
* from ASTNode->pRoot we collect only the necessary information and store in typetable
*
*	class  functions
*  ---------------------------------
*	std::unordered_map<std::string,_pair> doTypeAnal(); - makes use of ASTRef and DFS for searching and adding values to type table
*	void DFS(ASTNode* pNode); - function for recursively searching depth first through Abstract syntax tree
*	std::string setnSpace(std::string ns); - function for returning namespace string
*   bool doDisplay(ASTNode* pNode); - checking against array of strings, to show what to include in type table
*
*	Build Process:
*	==============
*	Required files
*	- AbstrSynTree.h, AbstrSynTree.cpp, TypeTable.h
*	- ActionsAndRules.h, ConfigureParser.h, ScopeStack.h
*	- Scanner.h
*
*	Build command
*	- devenv Project2.sln
*
* Maintenance History:
* --------------------
* Ver 1.0 : 07 Mar 2017
* - first release
*
*/

#include <iostream>
#include <string>
#include <unordered_map>
#include "../TypeTable/TypeTable.h"
#include "../CodeAnalyzer/ConfigureParser.h"
#include "../CodeAnalyzer/ActionsAndRules.h"

using namespace CodeAnalysis;

	class TypeAnalysis {
	public:
		using SPtr = std::shared_ptr<ASTNode*>;
		TypeAnalysis();
		std::unordered_map<std::string,_pair> doTypeAnal();
		std::string setnSpace(std::string ns);
	private:
		void DFS(ASTNode* pNode);
		bool doDisplay(ASTNode* pNode);
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
		TypeTable *tt = new TypeTable();
	};
