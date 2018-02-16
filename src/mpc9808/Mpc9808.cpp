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
	std::cout<<"I2c Opened "<<m_fileNode<<std::endl;

	//If we are here then lets set the slave Address
	 if (ioctl(m_fileNode, I2C_SLAVE, SLAVE_ADDRESS) < 0)
	 {
		 std::cout<<"Mpc9808::Mpc9808: Cannot set slave address = "<<SLAVE_ADDRESS<<std::endl;
		 close(m_fileNode);
		 m_fileNode = -1;
		 return;
	 }
}

bool Mpc9808::readTemperature(int& temperature)
{
	std::cout<<"file node =  "<<m_fileNode<<std::endl;
	if(m_fileNode < 0)
	{
		std::cout<<"Mpc9808::readTemperature: I2c not open"<<std::endl;
		return false;
	}
	/* Using SMBus commands */
	//auto res = i2c_smbus_read_word_data(m_fileNode, TEMPERATURE_REGISTER);
	//First write the address
	if(write(m_fileNode,(void*)&TEMPERATURE_REGISTER,sizeof(TEMPERATURE_REGISTER)) < 0)
	{
		std::cout<<"Mpc9808::readTemperature: Cannot set temperature address\n";
		return false;
	}

	//then read 2 bytes
	unsigned short rawTemperature;
	auto size = sizeof(rawTemperature);
	auto returnSize = read(m_fileNode,(void*)&rawTemperature,size);
	if(static_cast<unsigned int>(returnSize) != size)
	{
		std::cout<<"Mpc9808::readTemperature: Cannot Read temperature \n";
		return false;
	}

	//Lets Just print the value
	std::cout<<"Raw Temperature = 0x"<<std::hex<<rawTemperature<<std::dec<<std::endl;

	temperature = convertTemperature(rawTemperature);
	printf("Mpc9808::readTemperature: temperature = %d\n",temperature);

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
	//std::cout<<"Upper Byte = 0x"<<std::hex<<upperByte<<std::endl;
	//Then mask the flags
	upperByte &= MASK_FLAG;
	printf("Upper Byte = %d\n",upperByte);
	unsigned char lowerByte = (temperatureRaw >> 8) & MASK_BYTE;
	//std::cout<<"Lower Byte = 0x"<<std::hex<<lowerByte<<std::endl;
	printf("Lower Byte = %d\n",lowerByte);
	int numerator = upperByte*16;
	printf("numerator = %d\n",numerator);
	int denominator = lowerByte/16;
	printf("denominator = %d\n",denominator);
	int temperature = numerator+denominator;
	if(negative)
	{
		temperature -=256;
	}
	printf("Mpc9808::convertTemperature temperature = %d\n",temperature);
	return temperature;
}

Mpc9808::~Mpc9808()
{
	std::cout<<"Calling destructor for Mpc9808\n";
	if(m_fileNode > 0)
	{
		close(m_fileNode);
		m_fileNode = -1;
	}
}

