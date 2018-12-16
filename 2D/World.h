/*
 * World.h
 *
 *  Created on: Aug 19, 2016
 *      Author: Niels
 */

#ifndef WORLD_H_
#define WORLD_H_

using namespace std;

#include "../API/SimplexNoise/SimplexNoise.h"
#include <ctime>
#include <cmath>
#include <vector>
#include <sstream>

#include "BiomeManager.h"
#include "../Core/BlockManager.h"

//TODO: everything that is loaded must be a pointer; not a bloody instance!
//TODO: Also; it is weird float -> double works, double -> float doesn't. It takes care of some unused memory, but things look weird

class World{
public:
	struct Coord{
	public:
		unsigned int x, y;
		Coord(unsigned int i, unsigned int j) : x(i), y(j){}
		Coord(double _x, double _y) : x(_x < 0 ? 0 : _x), y(_y < 0 ? 0 : _y) {}
		bool operator==(Coord c){
			return c.x==x&&c.y==y;
		}
	};
	struct ChunkBiome {
	private:
		Biome *b;
		unsigned int bx, bex;
	public:
		ChunkBiome(Biome *_b, unsigned int _bx, unsigned int _bex) : b(_b), bx(_bx), bex(_bex) {}
		ChunkBiome() : ChunkBiome(nullptr, 0, 0) {}
		Biome *getBiome() { return b; }
		unsigned int getBiomeX() { return bx; }
		unsigned int getBiomeEndX() { return bex; }
		double getLength() { return bex - bx; }
		double getMiddle() { return bx + getLength()/2; }
		string toString() {
			if (b == nullptr)return "";
			stringstream ss;
			ss << b->getName().c_str() << " " << bx << "," << bex;
			return ss.str();
		}
		bool contains(double x) {
			return x >= bx && x < bex;
		}
		void extend(unsigned int i) {
			bex += i;
		}
	};
//	struct Node{
//	public:
//		struct NodeConnection{
//		public:
//			enum ConnectionStatus{
//				DEPRECATED=1<<0,
//				SEND_POWER=1<<1, SEND_DEATH=1<<2, SEND_LOCATION=1<<3, SEND_VISION=1<<4, SEND_CONTROLL=1<<5, SEND_REAL_LOCATION=1<<6,
//				RECEIVE_POWER=1<<4, RECEIVE_DEATH=1<<5, RECEIVE_LOCATION=1<<6, RECEIVE_VISION=1<<9, RECEIVE_CONTROLL=1<<10, RECEIVE_REAL_LOCATION=1<<7,
//				SHARE_POWER=1<<7, SHARE_DEATH=1<<8, SHARE_LOCATION=1<<9, SHARE_VISION=1<<13, SHARE_CONTROLL=1<<14, SHARE_REAL_LOCATION
//			};
//			enum ConnectionState{
//				COVEN = SHARE_POWER | SHARE_DEATH | SHARE_LOCATION,
//				LINK = SHARE_POWER | SEND_REAL_LOCATION,
//				BINDING = SHARE_DEATH | SHARE_LOCATION | SEND_POWER,
//				REMAINS = RECEIVE_POWER,
//				GHOST = SEND_CONTROLL | RECEIVE_VISION | RECEIVE_REAL_LOCATION,
//				SIRE = SEND_DEATH | SEND_CONTROLL | SEND_VISION | RECEIVE_VISION | SHARE_POWER,
//				PACK = RECEIVE_POWER | SEND_POWER | RECEIVE_REAL_LOCATION
//			};
//		private:
//			Node *parent, *child;
//			ConnectionState state;
//		};
//	private:
//		Coord location;
//		Node *proxy;
//		vector<NodeConnection> children;
//	public:
//		Node(Coord loc, Node *prox=nullptr) : location(loc), proxy(prox){}
//		void addChild(NodeConnection n){children.push_back(n);}
//		Coord trace(){
//			if(proxy==nullptr)return location;
//			Node *temp = proxy;
//			Coord res = proxy->location;
//			while(temp!=nullptr){
//				res = temp->location;
//				temp = temp->proxy;
//			}
//			return res;
//		}
//		Coord getLocation(){return location;}
//	};
	ChunkBiome *getChunkBiome(unsigned int i) {
		if (i >= w || biomes.size() == 0)return nullptr;
		for (size_t j = 0; j < biomes.size(); j++)
			if (i >= biomes[j]->getBiomeX() && i < biomes[j]->getBiomeEndX())return biomes[j];
		return biomes[0];
	}
private:
	unsigned int *map, w, h;
	vector<ChunkBiome*> biomes;
	vector<double*> layers;
	bool hasGenerated;
	SimplexNoise sn = SimplexNoise();
private:
	struct IdCount{
	public:
		unsigned int count, id;
		IdCount(unsigned int i, unsigned int c) : count(c), id(i){}
	};
	unsigned int lowest(unsigned int i, double *heightmap){
		double left = i>0?heightmap[i-1]:((unsigned int)0-1);
		double right = i<w-2?heightmap[i + 1]:((unsigned int)0-1);
		if(left<0)left = 0;
		if(right < 0)right = 0;
		if(left<right)return (unsigned int)left;
		return (unsigned int)right;
	}
	double randomdouble(){
		return rand()/32767.0;
	}
	unsigned int getFilledX() {
		unsigned int maxX=0;
		for (size_t i = 0; i < biomes.size(); i++)
			if (biomes[i]->getBiomeEndX() > maxX)maxX = biomes[i]->getBiomeEndX();
		return maxX;
	}
	Biome *getBiome(unsigned int i) {
		if (i >= w || biomes.size() == 0)return nullptr;
		for (size_t j = 0; j < biomes.size(); j++)
			if (i >= biomes[j]->getBiomeX() && i < biomes[j]->getBiomeEndX())return biomes[j]->getBiome();
		return biomes[0]->getBiome();
	}
	Biome *getLayeredBiome(unsigned int i, unsigned int j) {
		if (i >= w || biomes.size() == 0 || j >= h)return nullptr;
		for (size_t k = 1; k < layers.size(); k++) 
			if (layers[k][j] >= i) return biomes[k]->getBiome();
		return biomes[biomes.size()-1]->getBiome();
	}
	ChunkBiome *findAvailable(Biome *b) {
		unsigned int maxX = getFilledX();

		unsigned int maxBiomeSize = 1024, minBiomeSize = 512;
		unsigned int biomeSize = minBiomeSize + rand() % (maxBiomeSize - minBiomeSize);

		ChunkBiome *cb = new ChunkBiome(b, maxX, maxX + biomeSize);
		biomes.push_back(cb);
		return cb;
	}
	double getMaxHeight(unsigned int i) {
		if (i >= w)return 0;
		if (biomes.size() == 0)return 0;
		if (biomes.size() == 1)return biomes[0]->getBiome()->getMaxHeight();
		bool contains, right;
		unsigned int chunkX = 0;
		for (size_t j = 0; j<biomes.size(); j++)
			if (biomes[j]->contains(i)) {
				chunkX = j;
				right = i >= biomes[j]->getMiddle();
				contains = true;
				break;
			}
		if (!contains)return biomes[0]->getBiome()->getMaxHeight();
		if ((right && chunkX >= biomes.size() - 1) || (!right && chunkX < 1))return biomes[chunkX]->getBiome()->getMaxHeight();
		double firstX = biomes[chunkX]->getMiddle();
		double secondX = right ? biomes[chunkX + 1]->getMiddle() : biomes[chunkX - 1]->getMiddle();
		int centerX = (firstX + secondX) / 2;
		double percentage = (((double)i - centerX) / abs(firstX - centerX) + 1) / 2, inverse = 1 - percentage;
		ChunkBiome *mostLeft = right ? biomes[chunkX] : biomes[chunkX - 1];
		ChunkBiome *mostRight = right ? biomes[chunkX + 1] : biomes[chunkX];
		double val = mostLeft->getBiome()->getMaxHeight() * percentage + mostRight->getBiome()->getMaxHeight() * inverse;

		double originPerc = 1 - abs((i - biomes[chunkX]->getMiddle()) / biomes[chunkX]->getLength() * 2), invOrigin = 1 - originPerc;

		return originPerc * biomes[chunkX]->getBiome()->getMaxHeight() + invOrigin * val;
	}
	double getMinHeight(unsigned int i) {
		if (i >= w)return 0; 
		if (biomes.size() == 0)return 0; 
		if (biomes.size() == 1)return biomes[0]->getBiome()->getMinHeight();
		bool contains, right;
		unsigned int chunkX = 0;
		for(size_t j=0;j<biomes.size();j++)
			if (biomes[j]->contains(i)) {
				chunkX = j;
				right = i >= biomes[j]->getMiddle();
				contains = true;
				break;
			}
		if(!contains)return biomes[0]->getBiome()->getMinHeight();
		if ((right && chunkX >= biomes.size() - 1) || (!right && chunkX < 1))return biomes[chunkX]->getBiome()->getMinHeight();
		double firstX = biomes[chunkX]->getMiddle();
		double secondX = right ? biomes[chunkX + 1]->getMiddle() : biomes[chunkX - 1]->getMiddle();
		int centerX = (firstX + secondX) / 2;
		double percentage = (((double)i - centerX) / abs(firstX - centerX) + 1) / 2, inverse = 1 - percentage;
		ChunkBiome *mostLeft = right ? biomes[chunkX] : biomes[chunkX - 1];
		ChunkBiome *mostRight = right ? biomes[chunkX + 1] : biomes[chunkX]; 
		double val = mostLeft->getBiome()->getMinHeight() * percentage + mostRight->getBiome()->getMinHeight() * inverse;

		double originPerc = 1 - abs((i - biomes[chunkX]->getMiddle()) / biomes[chunkX]->getLength() * 2), invOrigin = 1 - originPerc;

		return originPerc * biomes[chunkX]->getBiome()->getMinHeight() + invOrigin * val;
	}

	void destruction() {
		delete[] map;
		for (size_t i = 0; i < layers.size(); i++)delete[] layers[i];
		for (size_t i = 0; i < biomes.size(); i++)delete biomes[i];
	}
public:
	World(unsigned int width, unsigned int height){
		map = new unsigned int[width*height];
		for(unsigned int i=0;i<width*height;i++)map[i] = 0;
		w = width;
		h = height;
		hasGenerated = false;
		//TODO: Load BlockManager from file (so it knows the basic values)
	}
	~World(){
		destruction();
	}
	unsigned int get(unsigned int x, unsigned int y){
		if(x>=w||y>=h)return 0;
		return map[y*w+x];
	}
	unsigned int get(Coord c){
		return get(c.x, c.y);
	}
	void set(unsigned int x, unsigned int y, unsigned int b){
		if(x>=w||y>=h)return;
		map[y*w+x] = b;
	}
	void set(Coord c, unsigned int b){
		set(c.x, c.y, b);
	}
	vector<Coord> getNeighbors(Coord c, unsigned int cap){
		vector<Coord> neighbor;
		unsigned int x = c.x;
		unsigned int y = c.y;
		if(x>0 && get(x-1,y)==0 && y<=cap)neighbor.push_back(Coord(x-1,y));
		if(x<w-2 && get(x+1,y)==0 && y<=cap)neighbor.push_back(Coord(x+1,y));
		if(y>0 && get(x,y-1)==0 && y-1<=cap)neighbor.push_back(Coord(x,y-1));
		if(y<h-2 && get(x,y+1)==0 && y+1<=cap)neighbor.push_back(Coord(x,y+1));
		return neighbor;
	}
	vector<Coord> getNeighbors(Coord c){
		vector<Coord> neighbor;
		for(double i=-1;i<=1;i++)
			for(double j=-1;j<=1;j++){
				if(i==0&&j==0)continue;
				double x = c.x + i;
				double y = c.y + j;
				if(x<0||y<0||x>=w||y>=h)continue;
				neighbor.push_back(Coord((unsigned int)x, (unsigned int)y));
			}
		return neighbor;
	}
	unsigned int getWidth(){return w;}
	unsigned int getHeight(){return h;}
	bool contains(vector<Coord> &v, Coord c){
		for(INT32 i=0;i<(INT32)v.size();i++)
			if(v[i]==c)return true;
		return false;
	}
	void generate(BiomeManager *bm, BlockManager *blm){
		Biome *biome = bm->pickRandom();
		if(biome==nullptr){
			cout << "Couldn't find a biome, so couldn't generate the world!" << endl;
			return;
		}

		if (hasGenerated) {
			destruction();
			hasGenerated = false;
		}

		clock_t t = clock();
		cout << "Generating biomes..." << endl;
		findAvailable(biome);
		while (getFilledX() < w)
			findAvailable(bm->pickAppropriate(biomes[biomes.size() - 1]->getBiome()));
		cout << "Finished generating biomes within " << (clock() - t) << "ms" << endl;

		for (size_t i = 0; i < biomes.size(); i++)cout << biomes[i]->getBiome()->getName() << endl;

		cout << "Generating layer biomes..." << endl;

		for (size_t i = 1; i < biomes.size(); i++) {
			double bSMi = -(double)(rand() % (int)(biomes[i-1]->getLength() / 16)) + rand() % (int)(biomes[i]->getLength() / 16);
			double perY = bSMi / h;

			double *noises = new double[h];
			double nmin = pow(10, 99), nmax = -pow(10, 99);
			for (int y = 0; y < h; y++) {
				double n = sn.tnoise(y*0.01, 0, 1, 3, 0.5, 1.5);
				noises[y] = n;
				if (n < nmin)nmin = n;
				if (n > nmax)nmax = n;
			}
			for (int y = 0; y < h; y++) {
				double n = noises[(int)y];
				noises[y] = (n + nmin) / (nmax - nmin);
			}

			for (int y = 0; y < h; y++) 
				noises[y] = perY * y + biomes[i - 1]->getBiomeEndX() + noises[y] * 10;

			layers.push_back(noises);
		}

		t = clock();
		double percWaterH = 0.6;
		double percLavaH = 0.05;
		unsigned int waterH = (unsigned int)(percWaterH * h);
		unsigned int lavaH = (unsigned int)(percLavaH * h);
		const unsigned int STONE = 3, SAND = 10, WATER = 12, LAVA = 11, MUD = 4, CLAY = 5, HARDENED_DIRT = 6, HARDENED_ROCK = 7;
		cout << "Generating heightmap..." << endl;
		double thyme = (double)time(0)/1000/60/60/24;
		cout << "With time " << thyme << endl;
		double *heightMap = new double[w];
		double max = pow(10, -99);
		double min = pow(10, 99);
		double sX = (double)rand()*rand();
		for(unsigned int i=0;i<w;i++){
			double temp = heightMap[i] = sn.tnoise(i+sX, thyme, 250.0, 4, 0.5, 2);
			if(temp>max)max = temp;
			if(temp<min)min = temp;
		}
		for (unsigned int i = 0; i < w; i++) {
			int minH = getMinHeight(i), maxH = getMaxHeight(i);
			heightMap[i] = -(heightMap[i] - min) / (min - max) * (maxH - minH) + minH;
		}
		cout << "Generating surface..." << endl;
		vector<Coord> water;
		for (unsigned int i = 0; i < w; i++) {
			double y = heightMap[i];
			unsigned int j =  y<0?0:(unsigned int)(y);
			for(unsigned int jj=j;jj>0;jj--){
				for(unsigned int jj=waterH;jj>j;jj--)set(i,jj,WATER);
				if(j+1<=waterH)water.push_back(Coord(i,j+1));
				unsigned int rockType = (double)jj>(double)j-15.0?MUD:(double)jj>(double)j-20.0?CLAY:(double)jj>(double)j-25.0?HARDENED_DIRT:STONE;
				//TODO: Layers
				if(jj==j||lowest(i,heightMap) < jj){
					if(jj==j)set(i,jj,lowest(i,heightMap)<waterH&&jj<=waterH?SAND:blm->byName(getLayeredBiome(i,jj)->getTopBlock()));
					else set(i,jj,lowest(i,heightMap)<waterH&&jj<=waterH?SAND: blm->byName(getLayeredBiome(i, jj)->getBottomBlock()));
					if(jj==1)set(i,0,rockType);
				}
				else{
					set(i,jj,rockType);
					if(jj==1)set(i,0,rockType);
				}
			}
			for (unsigned int jj = j - 1; jj > j - 3; jj--) {
				set(i, jj, lowest(i, heightMap) < waterH ? SAND : blm->byName(getLayeredBiome(i, jj)->getBottomBlock()));
			}
			if (j == 0) {
				set(i, 0, lowest(i, heightMap) < waterH ? SAND : blm->byName(getLayeredBiome(i, j)->getTopBlock()));
			}
			if(j==1)set(i,0,STONE);
		}

		cout << "Generating chasms..." << endl;
		double omin = min;
		double omax = max;
		max = pow(10, -99);
		min = pow(10, 99);
		double **heightm = new double*[w];
		for(unsigned int i=0;i<w;i++)heightm[i] = new double[h];
		double sY = (double)rand()*rand();
		sX = (double)rand()*rand();
		for(unsigned int i=0;i<w;i++)
			for(unsigned int j=0;j<h;j++){
				//TODO also layers; you have 3; front, middle and back. Middle is the layer that contains all mapObjects.
				double temp = heightm[i][j] = sn.tnoise(i*3+sX, j*3+sY, thyme, 300, 4, 0.5, 2);
				if(temp>max)max = temp;
				if(temp<min)min = temp;
			}
		double minCaveSpawn = 0.1, maxCaveSpawn = 0.5;

		for (unsigned int i = 0; i < w; i++) {
			double y = heightMap[i];
			for (unsigned int j = 0; j < y; j++) {
				double val = -(heightm[i][j] - min) / (min - max);
				double chance = minCaveSpawn + (maxCaveSpawn-minCaveSpawn)*(y - j) / y + (1-maxCaveSpawn)*(j <= lavaH ? 1 - ((double)j / lavaH) : 0);
				double delta = val - chance;
				if (delta <= 0) {
					if (get(i, j) != WATER) {
						if (j < lavaH)set(i, j, LAVA);
						else {
							if (get(i, j) == STONE) set(i, j, 0);
							else if (get(i, j) == HARDENED_DIRT)set(i, j, HARDENED_ROCK);
						}
					}
				}
				else if (delta <= 0.05) {
					if (get(i, j) == STONE)set(i, j, HARDENED_ROCK);
				}
			}
		}
		for (unsigned int i = 0; i < w; i++) {
			double y = heightMap[i];
			for (unsigned int j = 0; j < y; j++) {
				if (get(i, j) != HARDENED_ROCK)continue;
				double val = -(heightm[i][j] - min) / (min - max);
				double chance = 0.2 + 0.25*(y - j) / y + 0.55*(j <= lavaH ? 1 - ((double)j / lavaH) : 0);
					bool is = false;
					double rad = 2;
					for (double x = -rad; x <= rad && !is; x++)
						for (double y = -rad; y <= rad; y++) {
							double l = sqrt(pow(x, 2) + pow(y, 2));
							if (l > rad)continue;
							double cx = x + i;
							double cy = y + j;
							if (cx < 0 || cy < 0 || cx >= w || cy >= h)continue;
							unsigned int c = get(Coord((unsigned int)cx, (unsigned int)cy));
							if (c != 0 && c != LAVA)continue;
							is = true;
							break;
						}
					if (!is)
						set(i, j, j > y - 25 ? HARDENED_DIRT : STONE);
			}
		}
		for(unsigned int i=0;i<w;i++)delete[] heightm[i];
		delete[] heightm;
		cout << "Generating caves..." << endl;	//TODO:
		cout << "Generating trees..." << endl;
		for(unsigned int i=0;i<w;i++){
			unsigned int j = heightMap[i]<0?0:heightMap[i];
			Biome *bi = getLayeredBiome(i, j+1);
			if (bi == nullptr)continue;
			const unsigned int GRASS = blm->byName(bi->getTopBlock()), DIRT = blm->byName(bi->getBottomBlock());
			if(get(i,j)!=GRASS)continue;
			Tree t = bi->getTree();
			if(randomdouble() >= t.treesPerChunk() / 16.0)continue;
			double ra = t.randomRadius();
			unsigned int sy = t.randomLength();
			const unsigned int WOOD = blm->byName(t.getWood()), LEAVES = blm->byName(t.getLeaves());
			bool b = false;
			for(unsigned int jj=0;jj<ra+sy&&jj+j<h-1;jj++)if((t.canSurviveWater() ? get(i, j+jj+1) != WATER : get(i,j+jj+1)!=0) || get(i-1,j+jj+1)==WOOD){
				b=true;
				break;
			}
			if(b)continue;
			for(unsigned int jj=0;jj<ra+sy&&jj+j<h-1;jj++)set(Coord(i,j+jj+1), WOOD);
			if(t.getLeaves()!="Air"&&t.getLeaves()!="")
			for(double x=-ra+1;x<=ra-1;x++)
				for(double y=-ra+1;y<=ra-1;y++){
					double tx = x+i;
					double ty = y+ra+sy+j;
					if(tx<0||ty<0||tx>=w||ty>=h)continue;
					double l = sqrt(pow(x, 2) + pow(y,2));
					if(l>ra)continue;
					Coord c = Coord((unsigned int)tx,(unsigned int)ty);
					if (t.canSurviveWater())cout << "Setting leaves?" << endl;
					if(get(c)==0)set(c, LEAVES);
				}
		}
		cout << "Generating ores..." << endl;	//TODO:
		cout << "Generating structures..." << endl;	//TODO:
		cout << "Updating water..." << endl;
		while(water.size()!=0){
			vector<Coord> newest;
			for(unsigned int i=0;i<water.size();i++){
				if(get(water[i])==0)set(water[i],WATER);
				if(newest.size()<=900000){
					vector<Coord> neighbors = getNeighbors(water[i], waterH);
					for(unsigned int i=0;i<neighbors.size();i++)if(!contains(newest, neighbors[i]))newest.push_back(neighbors[i]);
				}else{
					if(!contains(newest, water[i]))newest.push_back(water[i]);
				}
			}
			water = newest;
		}
		water.clear();
		cout << "Hardening lava" << endl;
		for (unsigned int i = 0; i < w; i++) {
			double y = heightMap[i];
			for (unsigned int j = 0; j < y; j++) {
				if (get(i, j) != LAVA)continue;
				bool is = false;
				for (double x = -4; x <= 4 && !is; x++)
					for (double y = -4; y <= 4; y++) {
						double l = sqrt(pow(x, 2) + pow(y, 2));
						if (l > 4)continue;
						double cx = x + i;
						double cy = y + j;
						if (cx < 0 || cy < 0 || cx >= w || cy >= h)continue;
						unsigned int c = get(Coord((unsigned int)cx, (unsigned int)cy));
						if (c != WATER)continue;
						is = true;
						break;
					}
				if (is)set(i, j, HARDENED_ROCK);
			}
		}
		delete[] heightMap;
		cout << "Generated world! (Within " << (clock()-t) << "ms)" << endl;
	}
};

#endif /* 2D_WORLD_H_ */
