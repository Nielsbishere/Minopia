/*
 * BlockManager.h
 *
 *  Created on: Aug 31, 2016
 *      Author: Niels
 */

#ifndef BLOCKMANAGER_H_
#define BLOCKMANAGER_H_

using namespace std;

#include "Block.h"

string getRealName(string s){
	int cont = contains(s,'.');
	if(cont==-1)return s;
	return s.substr(cont+1);
}

class BlockManager{
private:
	struct BlockHolder{
	private:
		Block *b;
		string name;
	public:
		BlockHolder(string _name): b(nullptr), name(_name){}
		BlockHolder(Block *_b): b(_b), name(_b==nullptr?"":_b->getName()){}
		bool hasBlock(){return b!=nullptr;}
		string getName(){return name;}
		Block *getBlock(){return b;}
	};
	vector<BlockHolder> blocks;
public:
	void reserve(unsigned int i, string name){
		while(blocks.size()<=i)
			blocks.push_back(BlockHolder(name));
	}
	void set(unsigned int i, Block *b, string name){
		if(i>0)reserve(i-1, "");
		reserve(i, name);

		blocks[i] = b;
	}
	string getName(unsigned int i){
		if(blocks.size()<=i || !blocks[i].hasBlock() || i == 0)return "AIR";
		return blocks[i-1].getName();
	}
	unsigned int byName(string name){
		for(unsigned int i=0;i<blocks.size();i++)if(blocks[i].getName()==name)return i+1;

		for(unsigned int i=0;i<blocks.size();i++)if(blocks[i].hasBlock() && getRealName(blocks[i].getName()) == name){
			if(!blocks[i].getBlock()->isUniversal())continue;
			return i+1;
		}

		return 0;
	}
	Block *byId(unsigned int id) {
		if (id >= blocks.size() || !blocks[id].hasBlock())return nullptr;
		return blocks[id].getBlock();
	}

	void add(Block *b){
		if (b == nullptr)return;
		if(b->isUniversal())
			for(unsigned int i=0;i<blocks.size();i++)
				if(blocks[i].hasBlock() && getRealName(blocks[i].getName())==getRealName(b->getName()))return;
		for(unsigned int i=0;i<blocks.size();i++)
			if(!blocks[i].hasBlock() && (blocks[i].getName()==b->getName() || blocks[i].getName() == "")){
				blocks[i] = b;
				return;
			}
		cout << "Adding block " << b->getName().c_str() << " with id " << (blocks.size()+1) << endl;
		blocks.push_back(BlockHolder(b));
	}
	void reset(){blocks.clear();}
	string toString(){
		string s;
		for(unsigned int i=0;i<blocks.size();i++)s.append(i==0?"":",").append(blocks[i].getName());
		return s;
	}
	static void fromString(string s, BlockManager *bm){
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
		for(unsigned int i=0;i<names.size();i++)bm->reserve(i,names[i]);
	}
};


#endif /* 2D_BLOCKMANAGER_H_ */
