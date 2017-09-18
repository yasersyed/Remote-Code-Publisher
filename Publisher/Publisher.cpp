///////////////////////////////////////////////////////////////////
// Publisher.cpp - Publisher Library			                 
// Ver 1.0                                                       
// Application: Contains Functions and Test Stub for Publisher.h 
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015
// Author:      Syed Yaser Ahmed				                 
//              sysyed@syr.edu		                             
///////////////////////////////////////////////////////////////////

#include "Publisher.h"
#include "../CodeAnalyzer/Executive.h"
#include <vector>
#include <thread>
#include <algorithm>

//extracts file information from dependency table, creates html file with information received, 

void Publisher::generateHTML(std::unordered_map<std::string, std::vector<std::string>> DepTable, std::unordered_map <std::string, std::vector<std::pair<int, int>>> table,std::string folder)
{

	std::ofstream contentHTML; //Adding index.html, a file that contains path to all html files published
	contentHTML.open(folder +"index.html");
	contentHTML << htmlTag();
	contentHTML << headTag();
	contentHTML << "<ul>Pages created</ul>";
	std::cout << "\n Project 3 Requirement 5 & 6: CSS \"stylesheet.css\" and JavaScript \"bonustoggle.js\" included in each page's head \n";
	std::cout << " ---------------------------------------------------------------------------------------------------------- \n";
	std::cout << "\n Project 3 Requirement 7: Dependent files are shown as links \n";
	std::cout << " --------------------------------------------------- \n";
	std::cout << "\n Project 3 Requirement 9: Demonstrating Publisher by publishing important packages \n";
	std::cout << "\n ----------------------------------------------------------------------- \n";
	for (std::unordered_map<std::string, std::vector<std::string>>::const_iterator iter = DepTable.begin(); iter != DepTable.end(); ++iter)
	{
		//ofstream is used to create html files, data can be given as strings easily
		std::ofstream html;
		/*std::string filename = "./" + folder +"/"+ExtractFilename(iter->first);*/
		std::string filename = folder + ExtractFilename(iter->first);
		contentHTML << "<li><a href=\"" << ExtractFilename(iter->first) +".html"<<"\">"<< ExtractFilename(iter->first)<<"</a></li>";
		currPath = ExtractDirectory(filename);
		std::cout << "\n Generating HTML page " << filename << ".html\n";
		html.open(filename + ".html");
		html << addPrologues(ExtractFilename(iter->first));
		html << htmlTag(); 
		html << headTag();
		html << "<h3>"<<ExtractFilename(iter->first) << anchorTags(iter->first); 
		html << dependencies(iter->second); //Dependencies are added as links
		std::stringstream buffer1=generateCodeString(iter->first);//Adding code into pre tags
		std::stringstream buffer = optionalRequirement(iter->first,buffer1,table);
		html << buffer.rdbuf();
		html << closeBody();//closing html file
		html.close();
	}
	contentHTML << "</h3></body></html>";
	std::cout << "\n Project 3 Requirement 4: Classes and Functions can be Expanded or collapsed by clicking { in web pages \n";
	std::cout << " ---------------------------------------------------------------------------------------------- \n";
}


//Takes map of classes and functions along with their size in terms of lines of code 
//				  and adds toggle functionality to them

std::stringstream Publisher::optionalRequirement(std::string fileName,std::stringstream& buffer, std::unordered_map <std::string, std::vector<std::pair<int, int>>> table)
{
	std::string ostring = "<div class=\"div\">{</div><div class=\"toggle\">";
	std::string cstring = "</div>";
	std::string line;
	buffer.seekg(0, std::ios::end); //Gets stringstream of source file and checks with map
	#pragma warning(disable:4244)
	int size = buffer.tellg();
	buffer.seekg(0, std::ios::beg);
	std::stringstream returnbuf;
	std::vector<std::pair<int, int>> pairv = table[ExtractFilename(fileName)];
	std::vector<int> startline, endline; //Adding line numbers to startline,endline vectors
	for (auto it : pairv)
	{
		startline.push_back(it.first-1);
		endline.push_back(it.second-1);
	}
	for (int lineno = 0; std::getline(buffer, line) && lineno < size; lineno++)//Going through each line of stringstream to get to required lines
	{
		std::vector<int>::iterator it1,it2;//Adds div tag that enables toggle function in javascript
		it1 = std::find(startline.begin(), startline.end(), lineno);
		it2 = std::find(endline.begin(), endline.end(), lineno);
			if (it1 != startline.end())
			{
				if (line.find("{};")==std::string::npos && line.find("public:")==std::string::npos)
					line = "<div class=\"div\">" + line + "</div><div class=\"toggle\">";
				returnbuf << line << "\n";
			}
			else if (it2 != endline.end())
			{
				line += cstring;
				returnbuf << line << "\n";
			}
			else
				returnbuf << line << "\n";
	}
	return returnbuf;//returns modified stringstream
}

//Simple function to replace markup symbols and adding optional requirement functionality - collapsing and expanding everywhere opening and closing tags are found
std::stringstream Publisher::generateCodeString(std::string first)
{
	std::ifstream t(first);
	std::stringstream buffer;
	//std::string ostring = "<details><summary></summary>";
	//std::string cstring = "</details>}";
	std::string ostring = "<div class=\"div\">{</div><div class=\"toggle\">";
	std::string cstring = "</div>}";
	buffer << t.rdbuf();
	std::string myString(buffer.str());
	myString = ReplaceAll(myString, "<", "&lt;");
	myString = ReplaceAll(myString, ">", "&gt;");
	buffer.str(myString);
	return buffer;
}
//Removes trailing spaces, utility is not used in code
std::string Publisher::trim(std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
		return "";
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}
//Returns htmltag string
std::string Publisher::htmlTag()
{
	std::string htmltag;
	htmltag= "<!DOCTYPE html>\n<html>\n<head>\n\t";
	return htmltag;
}
//Returns headtag string
std::string Publisher::headTag()
{
	std::string headtag="<link rel=\"stylesheet\" type=\"text/css\" href=\"./Resources/stylesheet.css\"/>";
	headtag += "\n\t<script src=\" ./Resources/jquery-1.9.1.js \"></script>\n\t<script src=\"./Resources/bonustoggle.js\"></script>\n";
	headtag += "</head>\n<body>\n";
	return headtag;
}
//Returns anchortags string
std::string Publisher::anchorTags(std::string header)
{
	std::string anchortag = "</h3>\n<hr>\n<div class = ""indent"">";
	anchortag += "\n<ul>\n<li>You can download this file by clicking <a href=\"file:///";
	anchortag += header;
	anchortag += "\">here</a></li>\n<li>Optional Requirement: Click highlighted text to open global functions, methods and classes</li>\n</ul>\n<h4>Dependencies:</h4>\n";
	return anchortag;
}
//Returns dependencies string from vector of dependencies
std::string Publisher::dependencies(std::vector<std::string> DependentFiles)
{
	std::string deptags;
	if (DependentFiles.size() != 0)
	{
		for (auto it : DependentFiles)
		{
			deptags+= "<a href = """;
			deptags += ExtractFilename(it.c_str());
			deptags += ".html"">";
			deptags += ExtractFilename(it.c_str());
			deptags += "</a><br>\n";
		}
	}
	deptags += "</div>\n</hr>\n<hr>\n<pre>\n";
	return deptags;
}
//Returns closing tags as strings
std::string Publisher::closeBody()
{
	std::string closebody= "\n</pre>\n</hr>\n</body>\n</html>";
	return closebody;
}
//Returns html prologues, that can be viewed in source
std::string Publisher::addPrologues(std::string fileName)
{
	CodeAnalysis::CodeAnalysisExecutive exec;
	
	std::string prologue = "<!-----------------------------------------------------------------\n " + fileName +".html\n Published: ";
	prologue += exec.systemTime();
	prologue += "\n Syed Yaser Ahmed Syed, CS687 - Object Oriented Design, Spring 2017\n";
	prologue += "------------------------------------------------------------------>\n";
	return prologue;
}


//Replaces all instance of a string from another string by going through each character
std::string Publisher::ReplaceAll(std::string str, const std::string & from, const std::string & to)
{
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length();
	}
	return str;
}
//extracts directory information by using string find function
std::string Publisher::ExtractDirectory(const std::string& path)
{
	return path.substr(0, path.find_last_of('\\') + 1);
}
//extracts filename information by using string find function
std::string Publisher::ExtractFilename(const std::string& path)
{
	return path.substr(path.find_last_of('\\') + 1);
}
//Creates a start command with htmlFile name and executes the command by using std::system from windows.h library
void Publisher::OpenInBrowser(std::string htmlFile,std::string browser)
{
	std::string commandC = "start "+ browser +" \""+htmlFile+"\"";
	std::cout << "\n Opening " << htmlFile << "\n";
	std::system(commandC.c_str());
}

//Test stub
#ifdef DEBUG_PUBLISHER
int main()
{
	std::unordered_map<std::string, std::vector<std::string>> db;
	std::vector<std::string> testVector;
	testVector.push_back("FileB.h");
	testVector.push_back("FileB.cpp");
	testVector.push_back("FileC.h");
	testVector.push_back("FileD.cpp");
	db.emplace("FileA.h", testVector);
	Publisher pr;
	pr.generateHTML(db, "", "../FileA.h.html");
	return 0;
}
#endif // DEBUG_PUBLISHER