//============================================================================
// Name        : temperatureSensor.cpp
// Author      : Hemant Kapoor
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "mpc9808/Mpc9808.h"
using namespace std;

int main() {
	cout << "Application to read temperature sensor" << endl; // prints Application to read temperature sensor
	Mpc9808 myTempSensor(2);
	int currentTemperature(0);
	//Run loop 5 times
	int count=5;
	while(count-- !=0)
	{
		std::this_thread::sleep_for (std::chrono::seconds(3));
		if(myTempSensor.readTemperature(currentTemperature))
		{
			std::cout<<"Final Temperature = "<<currentTemperature<<std::endl;
		}
		else
		{
			std::cout<<"Error Finding Temperature\n";
		}
	}

	return 0;
}
