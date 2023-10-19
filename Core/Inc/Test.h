/*
 * Test.h
 *
 *  Created on: Sep 26, 2023
 *      Author: perrotg
 */

#ifndef SRC_TEST_H_
#define SRC_TEST_H_
#include <cstdio>
#include <cstdint>

class Test {
public:
	Test();
	virtual ~Test();
	void setData(uint8_t dataTmp);
private:
	std::uint8_t data;
};

#endif /* SRC_TEST_H_ */
