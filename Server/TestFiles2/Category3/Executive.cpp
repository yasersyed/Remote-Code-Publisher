/////////////////////////////////////////////////////////////////////
// Executive.cpp - Directs Code Analysis                           //
// ver 1.3                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2016                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2015 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////

#include "Executive.h"
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <exception>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <regex>
#include "Parser.h"
#include "FileSystem.h"
#include "FileMgr.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"
#include "AbstrSynTree.h"
#include "Logger.h"
#include "../Utilities/Utilities.h"
#include "../TypeAnalysis/TypeAnalysis.h"
#include "../TypeTable/TypeTable.h"
#include "../DependencyAnalyzer/DependencyAnalyzer.h"
#include "../Publisher/Publisher.h"

using Rslt = Logging::StaticLogger<0>;  // use for application results
using Demo = Logging::StaticLogger<1>;  // use for demonstrations of processing
using Dbug = Logging::StaticLogger<2>;  // use for debug output


/////////////////////////////////////////////////////////////////////
// AnalFileMgr class
// - Derives from FileMgr to make application specific file handler
//   by overriding FileMgr::file(), FileMgr::dir(), and FileMgr::done()

using Path = std::string;
using File = std::string;
using Files = std::vector<File>;
using Pattern = std::string;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;

using namespace CodeAnalysis;

//----< initialize application specific FileMgr >--------------------
/*
* - Accepts CodeAnalysisExecutive's path and fileMap by reference
*/
AnalFileMgr::AnalFileMgr(const Path& path, FileMap& fileMap)
	: FileMgr(path), fileMap_(fileMap), numFiles_(0), numDirs_(0) {}

//----< override of FileMgr::file(...) to store found files >------

void AnalFileMgr::file(const File& f)
{
	File fqf = d_ + "\\" + f;
	Ext ext = FileSystem::Path::getExt(fqf);
	Pattern p = "*." + ext;
	fileMap_[p].push_back(fqf);
	++numFiles_;
}
//----< override of FileMgr::dir(...) to save current dir >----------

void AnalFileMgr::dir(const Dir& d)
{
	d_ = d;
	++numDirs_;
}
//----< override of FileMgr::done(), not currently used >------------

void AnalFileMgr::done()
{
}
//----< returns number of matched files from search >----------------

size_t AnalFileMgr::numFiles()
{
	return numFiles_;
}
//----< returns number of dirs searched >----------------------------

size_t AnalFileMgr::numDirs()
{
	return numDirs_;
}

/////////////////////////////////////////////////////////////////////
// CodeAnalysisExecutive class
// - 
using Path = std::string;
using Pattern = std::string;
using Patterns = std::vector<Pattern>;
using File = std::string;
using Files = std::vector<File>;
using Ext = std::string;
using FileMap = std::unordered_map<Pattern, Files>;
using ASTNodes = std::vector<ASTNode*>;
using FileToNodeCollection = std::vector<std::pair<File, ASTNode*>>;

//----< initialize parser, get access to repository >----------------

CodeAnalysisExecutive::CodeAnalysisExecutive()
{
	pParser_ = configure_.Build();
	if (pParser_ == nullptr)
	{
		throw std::exception("couldn't create parser");
	}
	pRepo_ = Repository::getInstance();
	//ASTref_=Repository::getInstance()->AST();POST:Message
mode:oneway
toAddr:localhost::8080
fromAddr:localhost:8081
file:C:\Users\YaserSyed\Desktop\Project4\TestFiles\Executive.h
content-length:7495

#pragma once
/////////////////////////////////////////////////////////////////////
// Executive.h - Organizes and Directs Code Analysis               //
// ver 1.3                                                         //
//-----------------------------------------------------------------//
// Jim Fawcett (c) copyright 2016                                  //
// All rights granted provided this copyright notice is retained   //
//-----------------------------------------------------------------//
// Language:    C++, Visual Studio 2015                            //
// Platform:    Dell XPS 8900, Windows 10                          //
// Application: Project #2, CSE687 - Object Oriented Design, S2016 //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  ===================
*  Executive uses Parser, ScopeStack, and AbstrSynTree packages to
*  provide static code analysis for C++ and C# files contained in a
*  specified directory tree.
*
*  It finds files for analysis using FileMgr and, for each file found,
*  uses parser rules to detect:
*  - namespace scopes
*  - class scopes
*  - struct scopes
*  - control scopes
*  - function definition scopes
*  - data declarations
*  and uses rule actions to build an Abstract Sytax Tree (AST)
*  representing the entire code set's static structure.  Each detected
*  scope becomes a node in the AST which spans all of the files analyzed,
*  rooted in a top-level Global Namespace scope.
*
*  Executive supports displays for:
*  - a list of all files processed with their source lines of code count.
*  - an indented representation of the AST.
*  - a list of all functions that are defined, organized by package, along
*    with their starting lines, size in lines of code, and complexity as
*    measured by the number of their descendent nodes.
*  - a list of all functions which exceed specified function size and/or
*    complexity.
*
*  Because much of the important static structure information is contained
*  in the AST, it is relatively easy to extend the application to evaluate
*  additional information, such as class relationships, dependency network,
*  and static design flaws.
*
*  Required Files:
*  ---------------
*  - Executive.h, Executive.cpp
*  - Parser.h, Parser.cpp, ActionsAndRules.h, ActionsAndRules.cpp
*  - ConfigureParser.h, ConfigureParser.cpp
*  - ScopeStack.h, ScopeStack.cpp, AbstrSynTree.h, AbstrSynTree.cpp
*  - ITokenCollection.h, SemiExp.h, SemiExp.cpp, Tokenizer.h, Tokenizer.cpp
*  - IFileMgr.h, FileMgr.h, FileMgr.cpp, FileSystem.h, FileSystem.cpp
*  - Logger.h, Logger.cpp, Utilities.h, Utilities.cpp
*
*  Maintanence History:
*  --------------------
*  ver 1.3 : 28 Oct 2016
*  - added test for interface in AbstrSynTree functions show() and Add(ASTNode*)
*  - wrapped path in quotes for display
*  - added check for Byte Order Mark (BOM) in ConfigureParser.cpp
*  - wrapped path in quotes in Window.cpp in case path has spaces, necessary
*    since path is passed to Executive on command line.
*  ver 1.2 : 27 Oct 2016
*  - fixed bug in displayMetrics(ASTNode*) by adding test for interface
*  ver 1.1 : 28 Aug 2016
*  - many changes: added new functions and modified existing functions
*  ver 1.0 : 09 Aug 2016
*  - first release
*
*/
#include <string>
#include <vector>
#include <unordered_map>
#include <iosfwd>

#include "Parser.h"
#include "FileMgr.h"
#include "ConfigureParser.h"
#include "Utilities.h"

namespace CodeAnalysis
{
	using Utils = Utilities::StringHelper;

	///////////////////////////////////////////////////////////////////
	// AnalFileMgr class derives from FileManager::FileMgr
	// - provides application specific handling of file and dir events

	class AnalFileMgr : public FileManager::FileMgr
	{
	public:
		using Path = std::string;
		using File = std::string;
		using Files = std::vector<File>;
		using Pattern = std::string;
		using Ext = std::string;
		using FileMap = std::unordered_map<Pattern, Files>;

		AnalFileMgr(const Path& path, FileMap& fileMap);
		virtual void file(const File& f);
		virtual void dir(const Dir& d);
		virtual void done();
		size_t numFiles();
		size_t numDirs();
	private:
		Path d_;
		FileMap& fileMap_;
		bool display_;
		size_t numFiles_;
		size_t numDirs_;
	};

	///////////////////////////////////////////////////////////////////
	// CodeAnalysisExecutive class directs C++ and C# code analysis

	class CodeAnalysisExecutive
	{
	public:
		using Path = std::string;
		using Pattern = std::string;
		using Patterns = std::vector<Pattern>;
		using File = std::string;
		using Files = std::vector<File>;
		using Ext = std::string;
		using Options = std::vector<char>;
		using FileMap = std::unordered_map<Pattern, Files>;
		using FileNodes = std::vector<std::pair<File, ASTNode*>>;
		using Slocs = size_t;
		using SlocMap = std::unordered_map<File, Slocs>;
		CodeAnalysisExecutive();
		virtual ~CodeAnalysisExecutive();

		CodeAnalysisExecutive(const CodeAnalysisExecutive&) = delete;
		CodeAnalysisExecutive& operator=(const CodeAnalysisExecutive&) = delete;

		void showCommandLineArguments(int argc, char* argv[]);
		bool ProcessCommandLine(int argc, char* argv[]);
		FileMap& getFileMap();
		std::string getAnalysisPath();
		virtual void getSourceFiles();
		virtual void processSourceCode(bool showActivity);
		void complexityAnalysis();
		std::vector<File>& cppHeaderFiles();
		std::vector<File>& cppImplemFiles();
		std::vector<File>& csharpFiles();
		Slocs fileSLOCs(const File& file);
		size_t numFiles();
		size_t numDirs();
		std::string systemTime();
		virtual void displayMetrics(ASTNode* root);
		virtual void displayMetrics();
		virtual void displayMetricSummary(size_t sMa