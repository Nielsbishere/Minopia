/*
 * Tree.h
 *
 *  Created on: Sep 10, 2016
 *      Author: Niels
 */

#ifndef TREE_H_
#define TREE_H_

class Tree{
private:
	double treesPC;
	unsigned int miR, maR, miL, maL;
	string leaf, log;
	bool inWater;
public:
	Tree(double perChunk, unsigned int minR, unsigned int maxR, unsigned int minL, unsigned int maxL, string leaves, string wood, bool water): treesPC(perChunk), miR(minR), maR(maxR), miL(minL), maL(maxL), leaf(leaves), log(wood), inWater(water){}
	double treesPerChunk(){return treesPC;}
	double randomRadius(){return miR + (maR>miR?rand()%(maR-miR):0);}
	double randomLength(){return miL + (maL>miL?rand()%(maL-miL):0);}
	string getLeaves(){return leaf;}
	string getWood(){return log;}
	bool canSurviveWater() { return inWater; }

	static Tree def(){
		return Tree(1.2, 5, 6, 1, 2, "Leaves", "Wood", false);
	}
};

#endif /* 2D_TREE_H_ */
