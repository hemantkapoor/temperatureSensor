/*
 * Utility.cpp
 *
 *  Created on: 11 Feb 2018
 *      Author: hemant
 */

#include "Utility.h"
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include <iostream>
#include <iterator>


std::vector<std::string> Utility::parseString(std::string& str)
{
	if(str.empty())
	{
		return std::vector<std::string>();
	}
	std::stringstream ss(str);
	std::istream_iterator<std::string> begin(ss);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vectorString(begin, end);
	return vectorString;
}

int Utility::convertToNumber(const std::string& str)
{
	std::stringstream ss(str);
	int num;
	ss >> num;
	return num;
}

std::string Utility::convertToString(const int number)
{
	std::stringstream ss;
	ss << number;
	std::string str;
	ss >> str;
	return str;
}

bool Utility::writeToFile(const std::string& filePath,const std::string& value)
{
	//Open direction and write out or in
	auto fileNode = open(filePath.c_str(),O_WRONLY);
	if(fileNode == -1)
	{
		std::cout<<"Utility::writeToFile: Unable to open "<<filePath<<" file \n";
		return false;
	}
	write(fileNode,(void*)value.c_str(),sizeof(value));
	close(fileNode);
	return true;
}

bool Utility::writeToFile(const std::string& filePath,const int value)
{
	//Open direction and write out or in
	auto fileNode = open(filePath.c_str(),O_WRONLY);
	if(fileNode == -1)
	{
		std::cout<<"Utility::writeToFile: Unable to open "<<filePath<<" file \n";
		return false;
	}
	write(fileNode,(void*)&value,sizeof(value));
	close(fileNode);
	return true;
}
