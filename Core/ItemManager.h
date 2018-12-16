/*
 * ItemManager.h
 *
 *  Created on: Sept 2, 2016
 *      Author: Niels
 */

#ifndef ITEMMANAGER_H_
#define ITEMMANAGER_H_

using namespace std;

#include "Item.h"

string getMaterialName(string s){
	int cont = contains(s,'.');
	if(cont==-1)return s;
	return s.substr(cont+1);
}

class ItemManager{
private:
	struct ItemHolder{
	private:
		Item i;
		string name;
		bool hasI;
	public:
		ItemHolder(string _name): i(Item()), name(_name), hasI(false){}
		ItemHolder(Item _i): i(_i), name(_i.getName()), hasI(true){}
		bool hasItem(){return hasI;}
		string getName(){return name;}
		Item getItem(){return i;}
	};
	vector<ItemHolder> items;
public:
	void reserve(unsigned int i, string name){
		while(items.size()<=i)
			items.push_back(ItemHolder(name));
	}
	void set(unsigned int i, Item it){
		if(i>0)reserve(i-1, "");
		reserve(i, it.getName());

		items[i] = it;
	}
	string getName(unsigned int i){
		if(items.size()<=i || !items[i].hasItem() || i == 0)return "AIR";
		return items[i-1].getName();
	}
	unsigned int byName(string name){
		for(unsigned int i=0;i<items.size();i++)if(items[i].getName()==name)return i+1;
		for(unsigned int i=0;i<items.size();i++)if(items[i].hasItem() && getMaterialName(items[i].getName()) == name){
			if(!items[i].getItem().isUniversal())continue;
			return i+1;
		}
		return 0;
	}
	void add(Item it){
		if(it.isUniversal())
			for(unsigned int i=0;i<items.size();i++)
				if(items[i].hasItem() && getMaterialName(items[i].getName())==getMaterialName(it.getName()))return;
		for(unsigned int i=0;i<items.size();i++)
			if(!items[i].hasItem() && (items[i].getName()==it.getName() || items[i].getName() == "")){
				items[i] = it;
				return;
			}
		cout << "Adding item " << it.getName().c_str() << " with id " << (items.size()+1) << endl;
		items.push_back(ItemHolder(it));
	}
	void reset(){items.clear();}
	string toString(){
		string s;
		for(unsigned int i=0;i<items.size();i++)s.append(i==0?"":",").append(items[i].getName());
		return s;
	}
	static void fromString(string s, ItemManager *im){
		vector<string> names;
		string st;
		for(UINT32 i=0;i<s.size();i++){
			if(s[i]==','){
				names.push_back(st);
				st = "";
				continue;
			}
			st = st.append(string(new char{s[i]}));
		}
		for(unsigned int i=0;i<names.size();i++)im->reserve(i,names[i]);
	}
};


#endif /* 2D_BLOCKMANAGER_H_ */
