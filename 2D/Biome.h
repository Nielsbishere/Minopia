/*
 * Biome.h
 *
 *  Created on: Sept 10, 2016
 *      Author: Niels
 */

#ifndef BIOME_H_
#define BIOME_H_

using namespace std;

#include "Tree.h"

class Biome{
private:
	string name;
	bool universal;
	int minH, maxH;
	string topBlock, bottomBlock;
	vector<Tree> trees;
public:
	Biome(string _name, bool _universal, int _minH, int _maxH, string top, string bottom) : name(_name), universal(_universal), minH(_minH), maxH(_maxH), topBlock(top), bottomBlock(bottom){}
	Biome() : Biome("",false,0,0,"",""){}
	string getName(){return name;}
	bool isUniversal(){return universal;}
	int getMaxHeight(){return maxH;}
	int getMinHeight(){return minH;}
	double getAverageHeight() { return ((double)minH + maxH) / 2; }
	string getTopBlock(){return topBlock;}
	string getBottomBlock(){return bottomBlock;}
	void addTree(Tree t){trees.push_back(t);}
	Tree getTree(){if(trees.size()==0)return Tree::def();return trees[rand()%trees.size()];}
};



#endif /* BIOME_H_ */
