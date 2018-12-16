/*
 * InventorySlot.h
 *
 *  Created on: Sep 10, 2016
 *      Author: Niels
 */

#ifndef CORE_INVENTORYSLOT_H_
#define CORE_INVENTORYSLOT_H_

#include "InventoryAction.h"

class InventorySlot{
private:
	bool holdsItem;
	ItemStack *is;
	InventoryAction action;
	double x, y, width, height;
public:
	InventorySlot(ItemStack *_is, double _x, double _y, double _width, double _height): holdsItem(true), is(_is), x(_x), y(_y), width(_width), height(_height){}
	InventorySlot(InventoryAction _action, double _x, double _y, double _width, double _height): holdsItem(false), is(nullptr), action(_action), x(_x), y(_y), width(_width), height(_height){}
	~InventorySlot(){if(is!=nullptr)delete is;}
	ItemStack *getItemStack(){return is;}
	bool canHoldItem(){return holdsItem;}
	double getX(){return x;}
	double getY(){return x;}
	double getWidth(){return width;}
	double getHeight(){return height;}
};



#endif /* CORE_INVENTORYSLOT_H_ */
