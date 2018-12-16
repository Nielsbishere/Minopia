/*
 * Inventory.h
 *
 *  Created on: Sep 10, 2016
 *      Author: Niels
 */

#ifndef CORE_INVENTORY_H_
#define CORE_INVENTORY_H_

using namespace std;

#include "InventoryHandle.h"
#include "InventorySlot.h"

//TODO:
class Inventory{
private:
	string name;
	InventoryHandle *handle;
	InventorySlot *contents;
public:
	~Inventory(){if(handle!=nullptr)delete handle;if(contents!=nullptr)delete[] contents;}
};



#endif /* CORE_INVENTORY_H_ */
