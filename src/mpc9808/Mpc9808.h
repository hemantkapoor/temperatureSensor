/*
 * Mpc9808.h
 *
 *  Created on: 16 Feb 2018
 *      Author: hemant
 */

#ifndef MPC9808_H_
#define MPC9808_H_

class Mpc9808 {
public:
	Mpc9808()=delete;
	Mpc9808(int i2cNumber);
	bool readTemperature(int&);
	virtual ~Mpc9808();

private:
	int m_fileNode = -1;
	const std::string I2C_PATH = R"(/dev/i2c-)";
	const int SLAVE_ADDRESS = 0x18;
	//const int REGISTER_POINTER = 0x00;
	const unsigned char TEMPERATURE_REGISTER = 0x05;
	const unsigned short SIGN_BIT = (1 << 4);
	const unsigned short MASK_FLAG = 0x0f;
	const unsigned short NEGATIVE_FACTOR = 256;
	const unsigned short MASK_BYTE = 0x00ff;

	int convertTemperature(unsigned short);
};

#endif /* MPC9808_H_ */
