/*
 * BiomeManager.h
 *
 *  Created on: Sept 10, 2016
 *      Author: Niels
 */

#ifndef BIOMEMANGER_H
#define BIOMEMANGER_H

using namespace std;

#include "Biome.h"

class BiomeManager {
private:
	struct BiomeHolder {
	private:
		Biome *b;
		string name;
	public:
		BiomeHolder(string _name) : b(nullptr), name(_name) {}
		BiomeHolder(Biome *_b) : b(_b), name(_b==nullptr?"":_b->getName()) {}
		string getName() { return name; }
		Biome *getBiome() { return b; }
	};
	vector<BiomeHolder> biomes;
public:
	void reserve(unsigned int i, string name) {
		while (biomes.size() <= i)
			biomes.push_back(BiomeHolder(name));
	}
	void set(unsigned int i, Biome *b, string name) {
		if (i > 0)reserve(i - 1, "");
		reserve(i, name);

		biomes[i] = b;
	}
	string getName(unsigned int i) {
		if (biomes.size() <= i || biomes[i].getBiome() == nullptr)return "NULL";
		return biomes[i - 1].getName();
	}
	unsigned int byName(string name) {
		for (unsigned int i = 0; i < biomes.size(); i++)if (biomes[i].getName() == name)return i + 1;
		return 0;
	}
	void add(Biome *b) {
		if (b == nullptr)return;
		if (b->isUniversal())
			for (unsigned int i = 0; i < biomes.size(); i++)
				if (biomes[i].getBiome()!=nullptr && getRealName(biomes[i].getName()) == getRealName(b->getName()))return;
		for (unsigned int i = 0; i < biomes.size(); i++)
			if (biomes[i].getBiome() == nullptr && (biomes[i].getName() == b->getName() || biomes[i].getName() == "")) {
				biomes[i] = b;
				return;
			}
		cout << "Adding biome " << b->getName().c_str() << " with id " << biomes.size() << endl;
		biomes.push_back(BiomeHolder(b));
	}
	void reset() { biomes.clear(); }
	string toString() {
		string s;
		for (unsigned int i = 0; i < biomes.size(); i++)s.append(i == 0 ? "" : ",").append(biomes[i].getName());
		return s;
	}
	Biome *pickRandom() {
		if (biomes.size() == 0)return nullptr;
		return biomes[rand() % biomes.size()].getBiome();
	}
	double getNaturalValue(double val, double x, double y) {
		return -x * abs(pow(-x, -2) * 1 / -5) * pow(val - x, 2) + y;
	}
	double getValue(double val, double x) {
		return getNaturalValue(val, x, 1);
	}
	struct BiomeVal {
	private:
		Biome *b;
		double val;
	public:
		BiomeVal(Biome *_b, double _val) : b(_b), val(_val) {}
		double getVal() { return val; }
		Biome *getBiome() { return b; }
	};
	Biome *pickAppropriate(Biome *b) {
		if (biomes.size() == 0 || b==nullptr)return nullptr;
		if (biomes.size() == 1)return b;
		vector<BiomeVal> options;
		for (size_t i = 0; i < biomes.size(); i++) {
			Biome *biome = biomes[i].getBiome();
			if (biome->getName() == b->getName())continue;
			double naturalValue = getValue(biome->getAverageHeight(), b->getAverageHeight());
			if (naturalValue <= 0)continue;
			options.push_back(BiomeVal(biome, pow(100, naturalValue-0.5)/10));
		}
		if (options.size() == 0)return pickRandom();
		while (true) {
			BiomeVal random = options[rand() % options.size()];
			if (rand() / 32767.0 < random.getVal())continue;
			return random.getBiome();
		}
	}
	static void fromString(string s, BiomeManager *bm){
		vector<string> names;
		string st;
		for(size_t i=0;i<s.size();i++){
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


#endif /* 2D_BIOMEMANGER_H */
