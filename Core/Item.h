/*
 * Item.h
 *
 *  Created on: Sept 2, 2016
 *      Author: Niels
 */

#ifndef ITEM_H_
#define ITEM_H_

using namespace std;

class Item{
private:
	string name;
	bool universal;
public:
	Item(string _name, bool _universal) : name(_name), universal(_universal){}
	Item() : name(""), universal(false){}
	string getName(){return name;}
	bool isUniversal(){return universal;}
};



#endif /* 2D_BLOCK_H_ */
