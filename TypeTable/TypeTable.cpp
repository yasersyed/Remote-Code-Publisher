//////////////////////////////////////////////////////////////////////
// TypeTable.cpp - TypeTable Library								//
// Ver 1.0															//
// Application: Contains Test Stub for TypeTable.h					//
// Platform:    MacBook Pro, Win 10 Education, Visual Studio 2015	//
// Author:      Syed Yaser Ahmed									//
//              sysyed@syr.edu										//
//////////////////////////////////////////////////////////////////////

#include "TypeTable.h"
#include <Windows.h>

//Function to add data into type table, which is an unordered map
void TypeTable::addType(std::string filename, std::string ftype, std::string nspace)
{
	if (nspace != "main")
	{
		std::pair<std::string, std::string> pair = std::make_pair(ftype, filename);
		table.emplace(nspace, pair);
	}
}
//Function to return the stored table
std::unordered_map<std::string, _pair> TypeTable::returnMap()
{
	return table;
}

//Display function for displaying type table, not necessary for Project #3 but usage can be found in main() function
void TypeTable::displayMap()
{
	std::cout << std::setw(35) << std::left << "Name"
		<< std::setw(15) << "NameSpace" << "\t"
		<< std::setw(10) << "FilePath" << "\n";
	for (std::unordered_map<std::string, _pair>::const_iterator it = table.begin();
		it != table.end(); ++it)
	{
			std::cout << std::setw(35) << std::left << it->first
				<< std::setw(15) << it->second.first << "\t"
				<< std::setw(10) << it->second.second << "\n";
	}
}

//This function displays three periods and backspaces three periods to provide an animation
void TypeTable::loadAnimation()
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


#ifdef DEBUG_TYPETABLE
int main()
{
	TypeTable tb;
	tb.addType("abc", "type1", "path1");
	tb.addType("def", "type2", "path2");
	tb.addType("ghi", "type3", "path3");
	tb.addType("jkl", "type4", "path4");
	tb.addType("mno", "type5", "path5");
	tb.displayMap();
}
#endif // DEBUG