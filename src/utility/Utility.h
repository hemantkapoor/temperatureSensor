/*
 * Utility.h
 *
 *  Created on: 11 Feb 2018
 *      Author: hemant
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <vector>
#include <string>

class Utility {
public:
	//No constructor for singleton

	//Same goes for copy constructor
	Utility(const Utility&) = delete;
	//and assignment operator
	Utility& operator=(const Utility&) = delete;
	Utility() = delete;

	virtual ~Utility();
	static std::vector<std::string> parseString(std::string&);
	static int convertToNumber(const std::string&);
	static std::string convertToString(const int);
	static bool writeToFile(const std::string& filePath,const std::string& value);
	static bool writeToFile(const std::string& filePath,const int value);


private:
};

#endif /* UTILITY_H_ */
