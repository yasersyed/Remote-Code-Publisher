#pragma once

///////////////////////////////////////////////////////////////////////////
// DependencyAnalyzer.h									 					
//																		 
// Package for Analysing Dependency from a list of files				 
// Ver 1.0																 
// Application: can store a key and a pair of string values				 
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015		 
// Author:      Syed Yaser Ahmed										 
//              sysyed@syr.edu											 
///////////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This header contains class for comparing tokens from given files and matching them in Type table
* to get dependencies of given files
*
*	class  functions
*  ---------------------------------
*	unordered_map<string,vector<string>> findDependency(unordered_map<string, _pair>,vector<string>) -
*	Uses Abstract syntax tree and Scanner to get the tokens in each line of each file provided in the 1st argument.
*   The found tokens are compared to the type table map provided as first argument and builds a dependency unordered map.
*   
*	void LoadAnimation1() - simulates simple loading animation
*
*	Build Process:
*	==============
*	Required files
*	- AbstrSynTree.h, AbstrSynTree.cpp, TypeTable.h
*	- ActionsAndRules.h, ConfigureParser.h
*
*	Build commands
*	- devenv Project2.sln
*
* Maintenance History:
* --------------------
* Ver 1.0 : 07 Mar 2017
* - first release
*
*/


#include<iostream>
#include<unordered_map>
#include<vector>
#include<sstream>
#include"../TypeTable/TypeTable.h"
#include "../CodeAnalyzer/AbstrSynTree.h"
#include "../CodeAnalyzer/ActionsAndRules.h"
#include "../CodeAnalyzer/ConfigureParser.h"


using namespace Scanner;
namespace CodeAnalysis {
	class DependencyAnalyzer
	{
	public:
		DependencyAnalyzer();
		std::unordered_map<std::string, std::vector<std::string>> findDependency(std::unordered_map< std::string, _pair>,std::vector<std::string>);
		void loadAnimation1();
	private:
		std::unordered_map<std::string,std::string> dep;
		std::vector<std::pair<std::string,std::string>> deps;
		std::unordered_map<std::string, std::vector<std::string>> deps2;
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		Scanner::Toker& toker_;
	};
}