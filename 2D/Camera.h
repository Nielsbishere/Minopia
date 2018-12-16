/*
 * Camera.h
 *
 *  Created on: Aug 19, 2016
 *      Author: Niels
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#include <GL/freeglut.h>

#include "World.h"
#include "../RenderManager.h"
#include "../Core/BlockManager.h"

class Camera {
private:
	static RenderManager *render;
	World *w;
	bool movingRight, movingUp, movingHorizontal, movingVertical;
	double x, y, speed;
	bool isMovable, isBound, isAutoZoom;
	unsigned int rwi, rhe;
	//Rotation?
public:
	Camera(World *world, unsigned int _rwi, unsigned int _rhe, bool movable, bool zoom, bool bound=true) : w(world), speed(2), isMovable(movable), isBound(bound), isAutoZoom(zoom), rwi(_rwi), rhe(_rhe){movingRight=movingUp=movingHorizontal=movingVertical=false;x=y=0;}
	double getX(){return x;}
	double getY(){return y;}
	double getZoomX(){return rwi;}
	double getZoomY(){return rhe;}
	void draw() {
		unsigned int wi = rwi;
		unsigned int he = rhe;
		unsigned int mwi = w->getWidth();
		unsigned int mhe = w->getHeight();
		double perX = 2.0 / wi;
		double perY = 2.0 / he;

		if(x >= 2)return;
		if(y >= 2)return;
		if(x <= -(mwi*perX))return;
		if(y <= -(mhe*perY))return;

		double startX = (int)(-x/perX);
		if(startX<0)startX = 0;
		double maxX = (int)(-x/perX) + wi + 1;
		if(maxX<0)maxX = 0;

		double startY = (int)(-y/perY);
		if(startY<0)startY = 0;
		double maxY = (int)(-y/perY) + he + 1;
		if(maxY<0)maxY = 0;

		for (unsigned int i = (unsigned int)startX; i < (unsigned int)maxX && i < mwi; i++){
			for (unsigned int j = (unsigned int)startY; j < (unsigned int)maxY && j < mhe; j++) {
				unsigned int b = w->get(i, j);
				if (b == 0)continue;

				double sX = -1 + i * perX + x;
				double sY = -1 + j * perY + y;

				render->drawSprite(b, sX, sY, perX, perY);
			}
		}
	}
	void fix() {
		if (isBound) {
			if (x>0) {
				x = 0;
				movingHorizontal = false;
			}
			else if (x < -2.0 / rwi * w->getWidth() + 2) {
				x = -2.0 / rwi * w->getWidth() + 2;
				movingHorizontal = false;
			}
			if (y>0) {
				y = 0;
				movingVertical = false;
			}
			else if (y < -2.0 / rhe * w->getHeight() + 2) {
				y = -2.0 / rhe * w->getHeight() + 2;
				movingVertical = false;
			}
		}
	}
	void update(double deltaTime){
		if(movingHorizontal)x+=(movingRight?-1:1)*speed*deltaTime;
		if(movingVertical)y+=(movingUp?-1:1)*speed*deltaTime;
		fix();
	}
	void go(bool horizontal, bool b){
		if(!isMovable)return;
		if(horizontal){
			movingHorizontal = true;
			movingRight = b;
		}
		else{
			movingVertical = true;
			movingUp = b;
		}
	}
	void setSpeed(double x){
		speed = x;
	}
	void move(double dx, double dy){
		double perX = 2.0 / rwi;
		double perY = 2.0 / rhe;
		x+=perX*-dx;
		y+=perY*-dy;
		fix();
	}
	void stop(bool horizontal){
		if(!isMovable)return;
		if(horizontal)movingHorizontal = false;
		else movingVertical = false;
	}
	void zoom(int am){
		if(!isAutoZoom)return;
		//When zooming, make sure that it sticks to the same location;
		//-1 in x means:
		//-1/2 * zoom = blockX;
		//So to get that result for a different zoom;
		//blockX / zoom * 2 = xNew
		//Same for Y; just apply this; y = maxH - y;
		if(am<0){
			if(am+rwi<1)rwi = 1;
			else rwi+=am;
			if(am+rhe<1)rhe = 1;
			else rhe+=am;
		}else{
			if(am+rwi>w->getWidth())rwi = w->getWidth();
			else rwi+=am;
			if(am+rhe>w->getHeight())rhe = w->getHeight();
			else rhe+=am;
		}
	}
};

#endif /* 2D_CAMERA_H_ */
