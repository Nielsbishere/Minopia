/*
 * Block.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Niels
 */

#ifndef BLOCK_H_
#define BLOCK_H_

using namespace std;

class Block{
private:
	string name;
	bool liquid, universal;
public:
	Block(string _name, bool _universal, bool _liquid) : name(_name), universal(_universal), liquid(_liquid){}
	Block() : name(""), universal(false), liquid(false){}
	string getName(){return name;}
	bool isUniversal(){return universal;}
	bool isLiquid() { return liquid; }
};



#endif /* 2D_BLOCK_H_ */
