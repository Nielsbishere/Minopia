/*
 * Tag.h
 *
 *  Created on: Sep 10, 2016
 *      Author: Niels
 */

#ifndef CORE_TAG_H_
#define CORE_TAG_H_

using namespace std;

class Tag{
private:
	string name, value;
public:
	Tag(string _name, string _value): name(_name), value(_value){}
	string getName(){return name;}
	string getValue(){return value;}
};


#endif /* CORE_TAG_H_ */
