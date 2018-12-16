/*
 * ItemStack.h
 *
 *  Created on: Sep 10, 2016
 *      Author: Niels
 */

#ifndef CORE_ITEMSTACK_H_
#define CORE_ITEMSTACK_H_

#include "Block.h"
#include "Item.h"
#include "Tag.h"
#include <vector>

using namespace std;

class ItemStack{
private:
	Block *b;
	Item *i;
	unsigned int amount, data;
	vector<Tag> tags;
public:
	ItemStack(Block *_b, unsigned int _amount=1, unsigned int _data=0) : b(_b), i(nullptr), amount(_amount), data(_data){}
	ItemStack(Item *_i, unsigned int _amount=1, unsigned int _data=0) : b(nullptr), i(_i), amount(_amount), data(_data){}
	bool hasItem(){return i!=nullptr;}
	bool hasBlock(){return b!=nullptr;}
	void setTag(Tag t){
		size_t s = -1;
		for(size_t i = 0; i < tags.size(); i++)if(t.getName()==tags[i].getName()){
			s = i;
			break;
		}
		if(s!=-1){
			tags[s] = t;
			return;
		}
		tags.push_back(t);
	}
};



#endif /* CORE_ITEMSTACK_H_ */
