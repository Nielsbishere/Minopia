/*
 * core.cpp
 *
 *  Created on: Aug 19, 2016
 *  Last modified on:
 *      Author: Niels Brunekreef
 */

#define _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "freeglutd.lib")

#include <GL/freeglut.h>

#include <iostream>
#include <ctime>

#include "RenderManager.h"
#include "Core/BlockManager.h"
#include "Core/ItemManager.h"
#include "2D/BiomeManager.h"
#include "2D/Camera.h"
RenderManager blockRenderer = RenderManager();
RenderManager *Camera::render = &blockRenderer;

BlockManager blockManager = BlockManager();
ItemManager itemManager = ItemManager();
BiomeManager biomeManager = BiomeManager();
RenderManager itemRenderer = RenderManager();

#include "Mod/Parser.h"
Parser modParser = Parser();
#include "Core/State.h"
#include "2D/GameState.h"

#define SPACE 32
#define ESCAPE 27

using namespace std;

const static int MAX_FPS = 60;
const static int FRAME_PERIOD = 1000 / MAX_FPS;
double delta = 0;
bool init = false, initOnce = false;

int handle = 0;

const static time_t t = time(0);
State *state;

//TODO: Check why the mod takes up 23 MB instead of 58 kB? What the fuck is going on
//Maybe a memory leak somewhere
//TODO: Better rendering

void update(double deltaTime){
	if(state!=nullptr)state->update(deltaTime);
}
void draw(){
	if(state!=nullptr)state->draw();
	if (!init) {
		unsigned int dirt = blockManager.byName("Dirt"), stone = blockManager.byName("Stone");
		if (dirt != 0 || stone != 0) {
			for (float i = -1; i < 1; i += 0.1)
				for (float j = -1; j < 1; j += 0.1)
					blockRenderer.drawSprite((dirt != 0 && stone != 0) ? (rand() % 2 == 0 ? dirt : stone) : dirt != 0 ? dirt : stone, i, j, 0.1, 0.1);
		}
	}
}
void keypress(unsigned char ch, int x, int y){
	if(ch==ESCAPE)glutFullScreenToggle();
	if(state!=nullptr)state->keypress(ch);
}
void keynotpress(unsigned char ch, int x, int y){
	if(state!=nullptr)state->keynotpress(ch);
}
void click(int button, int s, int x, int y){
	if(s != GLUT_DOWN)return;
	double width = glutGet(GLUT_WINDOW_WIDTH);
	double height = glutGet(GLUT_WINDOW_HEIGHT);
	state->click(x/width, y/height, button);
}

#include "API/JSON/JSON.h"
const JSON::Priority JSON::debug = JSON::LOWEST;

void initialize(){
	delta = glutGet(GLUT_ELAPSED_TIME);
	clock_t t2 = clock(); 
	if (!initOnce) {
		srand(t);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_BLEND);
		modParser.parse("Minopia.json", &blockManager, &itemManager, &biomeManager, &blockRenderer, &itemRenderer);
		initOnce = true;
	}
	else {
		//Allocate about 1024 * 32 * 256 * 2 = 1024 * 64 * 256 = 64 * 256 kB = 16 MB of memory
		state = new GameState(1024 * 32, 256, &biomeManager, &blockManager);
		cout << "Initialized game state within " << (clock() - t2) << "ms" << endl;
		init = true;
	}
}
void cleanUp(){
	delete state;
}
void display() {
	   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	   glClear(GL_COLOR_BUFFER_BIT);
	   draw();
	   glFlush();
}

void loop(){
	if(!init)
		initialize();
	double deltaTime = (glutGet(GLUT_ELAPSED_TIME) - delta) / 1000;
	update(deltaTime);
	delta = glutGet(GLUT_ELAPSED_TIME);
	glutPostRedisplay();
}
//Initiating the window on start
int main(int argc, char** argv) {
   glutInit(&argc, argv); 
   glutInitWindowPosition(0, 0);
   glutInitWindowSize(640, 360);
   glutCreateWindow("Endless Creation by Osomi Lunar");
   glutDisplayFunc(display);
   glutIdleFunc(loop);
   glutKeyboardFunc(keypress);
   glutKeyboardUpFunc(keynotpress);
   glutCloseFunc(cleanUp);
   glutMouseFunc(click);
   handle = argc;
   glutMainLoop();
   return 0;
}
