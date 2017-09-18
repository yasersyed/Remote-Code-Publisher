#ifndef PUBLISHER_H
#define PUBLISHER_H
///////////////////////////////////////////////////////////////////
// Publisher.h - Contains class for generating html files 		 //
//																 //
// Ver 1.0                                                       //
// Application: can take a map of files and their dependencies	 //
//				generate html files with css and js				 //
//				for styles and scripts respectively				 //
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015//
// Author:      Syed Yaser Ahmed				                 //
//              sysyed@syr.edu		                             //
///////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This header contains functions for generating html files that display a files dependencies and its code
*	and adding bonus feature - minimizable functions, classes
* 
*
*	class  functions
*  ---------------------------------
*		void generateHTML(unordered_map<string, vector<string>> DepTable,std::unordered_map <std::string, std::vector<std::pair<int, int>>>); 
*				-extracts file information from dependency table
*		std::stringstream generateCodeString(std::string); 
*				-returns a buffer of code modified to add bonus feature
*		std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
*				-Replaces string inside code
*		std::string ExtractDirectory(const std::string & path);
*				-Get directory name of a file
*		std::string ExtractFilename(const std::string & path);
*				-Get filename of a file skipping the lengthy directory names
*		void OpenInBrowser(std::string htmlFile,std::string browser);
*				-Open a html file in browser of choice, first string is html file name, 2nd string is for browser selection
*		std::string trim(std::string& str);
*				- remove leading and trailing spaces from string
*		std::string htmlTag();
*				- Create html tag for HTML 5, returns a string
*		std::string headTag();
*				- Creates head tag, with cascading style sheet and javascript references given
*		std::string anchorTags(std::string);
*				- Creates anchor tags for notes
*		std::string dependencies(std::vector<std::string>);
*				- Creates list tags with anchors for dependencies
*		std::string closeBody();
*				- Returns a string of closing tabs
*		std::stringstream optionalRequirement(std::string,std::stringstream& newbuf,std::unordered_map <std::string, std::vector<std::pair<int, int>>> table);
*				- Takes map of classes and functions along with their size in terms of lines of code 
*				  and adds toggle functionality to them
*
* Maintenance History:
* --------------------
* Ver 1.0 : 28 Mar 2017
* - first release
*
*/

#include <iostream>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <Windows.h>

class Publisher
{
public:
	Publisher() {}
	~Publisher() {}
	void generateHTML(std::unordered_map<std::string, std::vector<std::string>> DepTable, std::unordered_map <std::string, std::vector<std::pair<int, int>>>,std::string folder);
	std::stringstream generateCodeString(std::string);
	std::string ReplaceAll(std::string str, const std::string& from, const std::string& to);
	std::string ExtractDirectory(const std::string & path);
	std::string ExtractFilename(const std::string & path);
	void OpenInBrowser(std::string htmlFile,std::string browser);
	std::string trim(std::string& str);
	std::string htmlTag();
	std::string headTag();
	std::string anchorTags(std::string);
	std::string dependencies(std::vector<std::string>);
	std::string closeBody();
	std::string addPrologues(std::string);
	std::stringstream optionalRequirement(std::string,std::stringstream& newbuf,std::unordered_map <std::string, std::vector<std::pair<int, int>>> table);
private:
	std::string currPath="";
};

#endif
