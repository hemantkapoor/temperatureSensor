/*
 * Mpc9808.cpp
 *
 *  Created on: 16 Feb 2018
 *      Author: hemant
 */

#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include <errno.h>
#include <stdio.h>
#include "../utility/Utility.h"
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "Mpc9808.h"

Mpc9808::Mpc9808(int i2cNumber)
{
	std::string i2cNumString = Utility::convertToString(i2cNumber);
	std::string filePath = I2C_PATH+i2cNumString;
	std::cout<<"Attempting to open "<<filePath<<std::endl;

	m_fileNode = open(filePath.c_str(),O_RDWR);
	//We try to open the i2c and set slave
	if(m_fileNode == -1)
	{
		std::cout<<"Mpc9808::Mpc9808: Unable to open "<<m_fileNode<<" file \n";
		std::cout<<errno<<std::endl;
		return;
	}
	//If we are here then lets set the slave Address
	 if (ioctl(m_fileNode, I2C_SLAVE, SLAVE_ADDRESS) < 0)
	 {
		 std::cout<<"Mpc9808::Mpc9808: Cannot set slave address = "<<SLAVE_ADDRESS<<std::endl;
		 close(m_fileNode);
		 m_fileNode = -1;
		 return;
	 }
	 //If everything goes as per the plan then create a thread
	 m_startThread = true;
	 m_thread = std::thread(&Mpc9808::readTemperaureThread,this);
}

bool Mpc9808::readTemperature(int& temperature)
{
	m_temperatureMutex.lock();
	temperature = m_currentTemperature;
	m_temperatureMutex.unlock();
	return true;


}

int Mpc9808::convertTemperature(unsigned short temperatureRaw)
{
	//First check the sign bit

	unsigned char upperByte = temperatureRaw & MASK_BYTE;
	bool negative(false);
	if(upperByte & SIGN_BIT)
	{
		std::cout<<"Got negative value \n";
		negative = true;
	}
	//Then mask the flags
	upperByte &= MASK_FLAG;
	unsigned char lowerByte = (temperatureRaw >> 8) & MASK_BYTE;
	int temperature = (upperByte*16)+(lowerByte/16);
	if(negative)
	{
		temperature -=256;
	}
	return temperature;
}

//Read temperature sensor every one second
void Mpc9808::readTemperaureThread()
{
	std::cout<<"Temperature sensor read thread started\n";
	while(m_startThread)
	{
		std::this_thread::sleep_for (std::chrono::seconds(1));
		if(m_fileNode < 0)
		{
			std::cout<<"Mpc9808::readTemperaureThread: I2c not open"<<std::endl;
			continue;
		}
		/* Using SMBus commands */
		//auto res = i2c_smbus_read_word_data(m_fileNode, TEMPERATURE_REGISTER);
		//First write the address
		if(write(m_fileNode,(void*)&TEMPERATURE_REGISTER,sizeof(TEMPERATURE_REGISTER)) < 0)
		{
			std::cout<<"Mpc9808::readTemperaureThread: Cannot set temperature address\n";
			continue;
		}

		//then read 2 bytes
		unsigned short rawTemperature;
		auto size = sizeof(rawTemperature);
		auto returnSize = read(m_fileNode,(void*)&rawTemperature,size);
		if(static_cast<unsigned int>(returnSize) != size)
		{
			std::cout<<"Mpc9808::readTemperaureThread: Cannot Read temperature \n";
			continue;
		}

		auto currentTemperature = convertTemperature(rawTemperature);
		m_temperatureMutex.lock();
		m_currentTemperature = currentTemperature;
		m_temperatureMutex.unlock();
	}
	if(m_fileNode > 0)
	{
		close(m_fileNode);
		m_fileNode = -1;
	}
}

Mpc9808::~Mpc9808()
{
	std::cout<<"Calling destructor for Mpc9808\n";
	if(m_startThread)
	{
		std::cout<<"Waiting for thread to be closed\n";
		//Stop the thread
		m_startThread = false;
		m_thread.join();
		std::cout<<"Thread closed...\n";
	}

}

