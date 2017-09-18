//////////////////////////////////////////////////////////////////////
 //DependencyAnalyzer.cpp - DependencyAnalyzer Library				
 //Ver 1.0															
 //Application: Contains Test Stub for DependencyAnalyzer.h			
 //Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015	
 //Author:      Syed Yaser Ahmed									
 //             sysyed@syr.edu										
//////////////////////////////////////////////////////////////////////


#include "DependencyAnalyzer.h"
#include <iostream>
#include <conio.h>
#include <iomanip>

//Constuctor gets AST tree, scope stack and tokers from repository
CodeAnalysis::DependencyAnalyzer::DependencyAnalyzer() :
	ASTref_(Repository::getInstance()->AST()),
	scopeStack_(Repository::getInstance()->scopeStack()),
	toker_(*(Repository::getInstance()->Toker()))
{
}

//Simulating progress by printing three Periods in an iterval and backspacing thrice
void CodeAnalysis::DependencyAnalyzer::loadAnimation1()
{
	Sleep(10);
	std::cout << "." << std::flush;
	Sleep(10);
	std::cout << "." << std::flush;
	Sleep(10);
	std::cout << "." << std::flush;
	Sleep(10);
	std::cout << "\b\b\b   \b\b\b" << std::flush;
}
//Creates dependency table by getting type table and vector of files to get dependencies of
std::unordered_map<std::string, std::vector<std::string>> CodeAnalysis::DependencyAnalyzer::findDependency(std::unordered_map< std::string, _pair> tab, std::vector<std::string> ve)
{
	std::cout << "\n Analyzing Dependencies Please Wait...";
	std::vector<std::string> CVector;
	//Iterating through vector of file paths
	for (std::string fileSpec : ve) {
		try
		{
			//Opening fileSpec and analysis tokens
			std::ifstream in(fileSpec);
			if (!in.good())
				std::cout << "\n  can't open " << fileSpec << "\n\n"; {
				Toker toker;
				toker.attach(&in);
				do {
					std::string tok = toker.getTok();
					//Matching token with the type table
					auto it = (tab.find(tok)); 
					if (it != tab.end()) {
						if ((fileSpec != tab[tok].second) && (std::find(deps.begin(), deps.end(), make_pair(fileSpec, tab[tok].second)) == deps.end())) {
							CVector.push_back(tab[tok].second);
							//If found then add into map
							deps.push_back(make_pair(fileSpec, tab[tok].second)); 
						}
					}
				} while (in.good());
				deps2[fileSpec] = CVector;//CVector contains children
				CVector = {};
			}
		}
		catch (std::logic_error& ex) {
			std::cout << "\n  " << ex.what();
		}
		loadAnimation1();
	}
	std::cout << "\n\n Dependency Analysis Completed!\n";
	return deps2;
}

#ifdef TEST_DEPENDENCY
int main()
{

	CodeAnalysis::DependencyAnalyzer da;
	std::unordered_map<std::string, _pair> maTable;
	maTable.emplace("test", std::make_pair("test1", "test2"));
	maTable.emplace("test8", std::make_pair("test3", "test4"));
	maTable.emplace("test9", std::make_pair("test5", "test6"));
	std::vector<std::string> veC;
	veC.push_back("test1");
	veC.push_back("test2");
	veC.push_back("test3");
	NoSqlDb<std::string> db=da.findDependency(maTable,veC);
}
#endif