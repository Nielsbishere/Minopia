/*
 * Mod.h
 *
 *  Created on: Aug 30, 2016
 *      Author: Niels
 */

#ifndef MOD_MOD_H_
#define MOD_MOD_H_

using namespace std;
#include <vector>

class Mod{
private:
	string name, publisher, description, version;
	unsigned int id;
	vector<string> authors;
	bool universal;
public:
	Mod(string _name, unsigned int _id, string _version, string _desc, string _creator, vector<string> _authors, bool _universal=true): name(_name), publisher(_creator), description(_desc), version(_version), id(_id), authors(_authors), universal(_universal){}
	string getName(){return name;}
	bool isUniversal(){return universal;}
};

#endif /* MOD_MOD_H_ */
