/*
 * Parser.h
 *
 *  Created on: Aug 30, 2016
 *      Author: Niels
 */

#ifndef MOD_PARSER_H_
#define MOD_PARSER_H_

using namespace std;
#include <iostream>
#include <vector>
#include <stdio.h>
#include <ctime>

#include "../API/JSON/JSON.h"
#include "../API/Base64/Base64.h"

#include "../RenderManager.h"
#include "../Core/BlockManager.h"
#include "../Core/ItemManager.h"
#include "../2D/BiomeManager.h"
#include "../2D/Tree.h"

#include "Mod.h"

class Parser{
private:
	vector<Mod> mods;
public:
	int parse(string _json, BlockManager *bm, ItemManager *im, BiomeManager *biom, RenderManager *rm, RenderManager *irm){
		clock_t t0 = clock();
		cout << "Received request to parse " << _json << endl;
		JSON json = JSON::read(_json);
		string name = json._str("name");
		if(name.size()<1){
			cout << "Couldn't get the mod name!" << endl;
			return -1;
		}
		string desc = json._str("description");
		if(desc.size()<1){
			cout << "Couldn't get the mod description!" << endl;
			return -1;
		}
		string version = json._str("version");
		if(version.size()<1){
			cout << "Couldn't get the mod version!" << endl;
			return -1;
		}
		string creator = json._str("publisher");
		if(creator.size()<1){
			cout << "Couldn't get the mod publisher!" << endl;
			return -1;
		}
		vector<string> authors = json._strvec("authors");
		if(authors.size()<1){
			cout << "Couldn't get the mod authors!" << endl;
			return -1;
		}
		bool universal = !json.hasKey("universal") || json._bool("universal");
		cout << "Parsing mod " << name << " with tempId " << mods.size() << endl;
		for(unsigned int i = 0;i<mods.size();i++)if(mods[i].getName() == name){
			cout << "Couldn't add " << name << " because there is already a mod with the same name!" << endl;
			return -1;
		}
		cout << "Parsed the mod with id " << mods.size() << endl;
		cout << "Further information: " << endl;
		cout << name << " version " << version << " by " << creator << endl;
		cout << desc << endl;
		cout << "Made possible by: ";
		for(unsigned int i=0;i<authors.size();i++)cout << authors[i] << " ";
		cout << endl << endl;

		Mod m = Mod(name, mods.size(), desc, version, creator, authors, universal);
		mods.push_back(m);

		cout << "Loading blocks..." << endl;
		JSON blocks = json._json("blocks");
		for(int i=0;i<blocks.getKeys()-1;i++){
			string s = blocks.getKey(i);
			bool hasUniversal = blocks._json(s).hasKey("universal");
			bool isUniversal = hasUniversal?blocks._bool(string(s).append(".universal")):false;
			string fullName = string("b:").append(name).append(".").append(s);
			Block *b = new Block(fullName, !hasUniversal?m.isUniversal():isUniversal, !blocks.hasKey(string(s).append(".liquid")) || blocks._bool(string(s).append(".liquid")));
			bm->add(b);
			rm->reserve(bm->byName(fullName), fullName);
		}
		cout << "Loaded blocks!" << endl << endl;

		cout << "Loading items..." << endl;
		JSON items = json._json("items");
		for(int i=0;i<items.getKeys()-1;i++){
			string s = items.getKey(i);
			bool hasUniversal = items._json(s).hasKey("universal");
			bool isUniversal = hasUniversal?items._bool(string(s).append(".universal")):false;
			string fullName = string("i:").append(name).append(".").append(s);
			Item it = Item(fullName, !hasUniversal?m.isUniversal():isUniversal);
			im->add(it);
			irm->reserve(im->byName(fullName), fullName);
		}
		cout << "Loaded items!" << endl << endl;

		cout << "Loading biomes..." << endl;
		JSON biomes = json._json("biomes");
		for(int i=0;i<biomes.getKeys()-1;i++){
			string s = biomes.getKey(i);
			JSON biome = biomes._json(s);
			bool hasUniversal = biome.hasKey("universal");
			bool isUniversal = hasUniversal?biome._bool("universal"):false;
			string fullName = string("w:").append(name).append(".").append(s);
			int minH = biome._int("terrain.minHeight");
			int maxH = biome._int("terrain.maxHeight");
			string top = biome._str("layers.top");
			string bottom = biome._str("layers.bottom");
			Biome *b = new Biome(fullName, !hasUniversal?m.isUniversal():isUniversal, minH, maxH, top, bottom);

			JSON::JSONList trees = biome._list("trees");
			for(int i=0;i<trees.getObjects();i++){
				if(trees.getType(i)!="JSON")continue;
				JSON *j = static_cast<JSON*>(trees.getObject(i));
				b->addTree(Tree(j->_double("perChunk"), j->_int("minRadius"), j->_int("maxRadius"), j->_int("minLength"), j->_int("maxLength"), j->_str("leaves"), j->_str("wood"), j->_bool("inWater")));
			}

			biom->add(b);
		}
		cout << "Loaded biomes!" << endl << endl << endl;

//		m.parseCrafting(json._json("crafting"));
//		m.parseSmelting(json._json("smelting"));

		clock_t t = clock();
		Base64 b = Base64();
		cout << "Loading block textures..." << endl;
		{
		JSON j = json._json("sources.textures.blocks");
		for(int i=0;i<j.getKeys()-1;i++){
			string k = j.getKey(i);
			string path = string("temp/b.").append(string(k).append(".png"));
			string val = j._str(k);
			if(!startsWith(val,"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAA")){
				cout << "String didn't use the right header!" << endl;
				continue;
			}
			val = val.substr(string("data:image/png;base64,").size());
			b.write(path, val);
			if (rm->addTexture(path.substr(0, path.size() - 4), &m, k, bm->byName(string("b:").append(m.getName()).append(".").append(k)), "b"))
				cout << "Loaded " << k << " into texture memory!" << endl;
			else
				cout << "Couldn't load " << k << " into texture memory" << endl;
			remove(path.c_str());
		}
		}
		cout << "Loaded block textures within " << (clock()-t) << "ms!" << endl << endl;

		t = clock();
		cout << "Loading item textures..." << endl;
		{
		JSON j = json._json("sources.textures.items");
		for(int i=0;i<j.getKeys()-1;i++){
			string k = j.getKey(i);
			cout << "Started parsing " << i << " " << k << endl;
			string path = string("temp/i.").append(k.append(".png"));
			string val = j._str(k);
			if(!startsWith(val,"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAA")){
				cout << "String didn't use the right header!" << endl;
				continue;
			}
			val = val.substr(string("data:image/png;base64,").size());
			b.write(path, val);
			irm->addTexture(path.substr(0,path.size()-4), &m, k.substr(0,k.size()-4), im->byName(string("i:").append(m.getName()).append(".").append(k.substr(0,k.size()-4))), "i");
			remove(path.c_str());
			cout << "Loaded " << k << " into texture memory!" << endl;
		}
		cout << "Loaded item textures within " << (clock()-t) << "ms!" << endl << endl;
		}

		cout << "Loaded/finalized mod within " << (clock()-t0) << "ms!" << endl;
		return mods.size();
	}
};

#endif /* MOD_PARSER_H_ */
