/*
 * GameState.h
 *
 *  Created on: Aug 19, 2016
 *      Author: Niels
 */

#ifndef GAMESTATE_H_
#define GAMESTATE_H_

#define UP 'w'
#define DOWN 's'
#define RIGHT 'd'
#define LEFT 'a'

#define ZOOMIN '='
#define ZOOMOUT '-'

#define REGENERATE '0'

#include "BiomeManager.h"
#include "../Core/BlockManager.h"

class GameState : public State{
private:
	World w;
	Camera c;
	BiomeManager *bm;
	BlockManager *blm;
public:
	GameState(unsigned int width, unsigned int height, BiomeManager *_bm, BlockManager *_blm) : w(World(width,height)), c(Camera(&w, 64, 36, true, true)), bm(_bm), blm(_blm){
		w.generate(bm,blm);
		c.setSpeed(10);
		c.move(w.getWidth()/2.0, w.getHeight()/3.0);
	}
	void update(double deltaTime){
		c.update(deltaTime);
	}
	virtual void click(double percX, double percY, int button){
		double blockX = c.getZoomX() * percX - c.getX()/2 * c.getZoomX();
		double blockY = c.getZoomY() * (1.0-percY) - c.getY()/2 * c.getZoomY();
		std::cout << "Clicked at " << blockX << "," << blockY << ";" << w.getChunkBiome(blockX)->toString().c_str() << " and isRight? " << (blockX >= w.getChunkBiome(blockX)->getMiddle()) << endl;
		w.set(World::Coord(blockX,blockY), 0);
	}
	virtual void draw(){
		c.draw();
	}
	virtual void keypress(unsigned char ch){
		if(ch==UP)c.go(false, true);
		else if(ch==DOWN)c.go(false, false);
		else if(ch==RIGHT)c.go(true, true);
		else if(ch==LEFT)c.go(true, false);

		if(ch==ZOOMOUT)c.zoom(1);
		else if(ch==ZOOMIN)c.zoom(-1);

		if(ch==REGENERATE){
			cout << endl << "Generating world..." << endl;
			w.generate(bm,blm);
		}
	}
	virtual void keynotpress(unsigned char ch){
		if(ch==UP||ch==DOWN)c.stop(false);
		else if(ch==RIGHT||ch==LEFT)c.stop(true);
	}
};



#endif /* 2D_GAMESTATE_H_ */
