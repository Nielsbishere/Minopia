/*
 * InventoryHandle.h
 *
 *  Created on: Sep 10, 2016
 *      Author: Niels
 */

#ifndef CORE_INVENTORYHANDLE_H_
#define CORE_INVENTORYHANDLE_H_

class InventoryHandle{
private:
public:
	virtual ~InventoryHandle(){}
	virtual bool checkHandle(double x, double y){return false;}
};



#endif /* CORE_INVENTORYHANDLE_H_ */
