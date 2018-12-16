/*
 * RenderManager.h
 *
 *  Created on: Aug 20, 2016
 *      Author: Niels
 */

#ifndef RENDERMANAGER_H_
#define RENDERMANAGER_H_

using namespace std;

#include <GL/freeglut.h>
#include "API/SOIL/SOIL2.h"
#include "Mod/Mod.h"

#include <vector>

int contains(string s, char c){
	for(unsigned int i=0;i<s.size();i++)if(s[i]==c)return i;
	return -1;
}
string getHandle(string s){
	if(s.size()>1 && s[1] == ':')return string(new char{s[0]});
	return "";
}

string getName(string s){
	if(s.size()>1 && s[1] == ':')return s.substr(2);
	return s;
}
string getMod(string s){
	int cont = contains(s,'.');
	if(cont==-1)return "";
	return s.substr(0, cont);
}
string getTName(string s){
	int cont = contains(s,'.');
	if(cont==-1)return s;
	return s.substr(cont+1);
}

class RenderManager {
public:
	struct Texture{
	private:
		GLuint id;
		string material;
		bool hasTexture;
		string getRName(){
			if(material.size()>1 && material[1] == ':')return material.substr(2);
			return material;
		}
	public:
		Texture(GLuint _id, string _material) : id(_id), material(_material), hasTexture(_id!=0){}
		Texture(string reserve) : Texture(0, ""){}
		bool operator==(Texture t){return t.id==id&&t.material==material;}
		GLuint getTexture(){return id;}
		string getHandle(){
			if(material.size()>1 && material[1] == ':')return string(new char{material[0]});
			return "";
		}
		string getMod(){
			string s = getRName();
			int cont = contains(s,'.');
			if(cont==-1)return "";
			return s.substr(0, cont);
		}
		string getName(){
			string s = getRName();
			int cont = contains(s,'.');
			if(cont==-1)return s;
			return s.substr(cont+1);
		}
		bool canDraw(){return hasTexture;}
	};
	void reserve(unsigned int i, string name){
		while(sprites.size()<i)sprites.push_back(nullptr);
		if(sprites.size()==i)sprites.push_back(new Texture(name));
	}
private:
	vector<Texture*> sprites;
	void drawTexturedRectangle(unsigned int t, double x, double y, double w, double h, double transparency){
		if(t>=sprites.size() || sprites[t] == nullptr || !sprites[t]->canDraw())return;
		GLuint texture = sprites[t]->getTexture();
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glColor4f(1, 1, 1, transparency);

		glBegin(GL_QUADS);
		glTexCoord2i(0, 0);glVertex2f(x, y);
		glTexCoord2i(1, 0);glVertex2f(x+w, y);
		glTexCoord2i(1, 1);glVertex2f(x+w, y+h);
		glTexCoord2i(0, 1);glVertex2f(x, y+h);
		glEnd();
	}
//	void addTexture(GLuint texture, string name, Mod *m, string handle){
//		sprites.push_back(Texture(texture, handle.append(":").append(m->getName()).append(".").append(name)));
//	}
	void setTexture(unsigned int i, GLuint texture, string name, Mod *m, string handle, bool log=false){
		if(log)cout << "Started setting " << i << endl;
		if(i<1 || i>=sprites.size() || sprites[i]==nullptr)return;
		string realName = handle.append(":").append(m->getName()).append(".").append(name);
		if(sprites[i]->canDraw() || (sprites[i]->getName()!="" && sprites[i]->getName()!=realName))return;
		delete sprites[i];
		sprites[i] = new Texture(texture, realName);
		if(log)cout << "Finished setting " << i << endl;
	}
	static GLuint loadPng(string file){
		GLuint texture = SOIL_load_OGL_texture(file.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_NTSC_SAFE_RGB);
		if(texture==0)return 0;
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		return texture;
	}
public:
	RenderManager() {
		reserve(0, "AIR");
	}
//	void addTexture(string file, Mod *m, string name, string handle="", bool log=true){
//		file = file.append(".png");
//		if(log)cout << "Loading " << file << endl;
//		GLuint texture = loadPng(file);
//		if(texture==0)return;
//		addTexture(texture, name, m, handle);
//		if(log)cout << "Loaded " << file << " (" << name << ") " << "with id " << texture << " and texture id " << (texture-1) << endl;
//	}
	bool addTexture(string file, Mod *m, string name, unsigned int i, string handle="", bool log=true){
		string fname = handle;
		reserve(i, fname.append(":").append(m->getName()).append(".").append(name));
		file = file.append(".png");
		if(log)cout << "Loading " << file.c_str() << endl;
		GLuint texture = loadPng(file);
		if(texture==0)return false;
		setTexture(i, texture, name, m, handle);
		if(log)cout << "Loaded " << file.c_str() << " "<< fname.c_str() << " (" << name.c_str() << ") " << "with sprite id " << texture << " and id " << i << endl;
		return true;
	}

	void drawSprite(unsigned int i, double x, double y, double w, double h, double transparency=1){
		if(i==0)return;
		drawTexturedRectangle(i, x, y, w, h, transparency);
	}
	//Deprecated due to lag
//	void drawSprite(string name, double x, double y, double w, double h){
//		if(name=="AIR")return;
//		string handle = getHandle(name);
//		string mod = getMod(getName(name));
//		string sprite = getTName(getName(name));
//		if(handle==""){
//			if(mod==""){
//				for(unsigned int i=0;i<sprites.size();i++)
//					if(sprites[i].getName()==sprite){
//						drawTexture(i,x,y,w,h);
//						return;
//					}
//			}else{
//				for(unsigned int i=0;i<sprites.size();i++)if(sprites[i].getName()==sprite && sprites[i].getMod()==mod){
//					drawTexture(i,x,y,w,h);
//					return;
//				}
//			}
//		}else{
//			if(mod==""){
//				for(unsigned int i=0;i<sprites.size();i++)if(sprites[i].getName()==sprite && sprites[i].getHandle()==handle){
//					drawTexture(i,x,y,w,h);
//					return;
//				}
//			}else{
//				for(unsigned int i=0;i<sprites.size();i++)if(sprites[i].getName()==sprite && sprites[i].getHandle()==handle && sprites[i].getMod()==mod){
//					drawTexture(i,x,y,w,h);
//					return;
//				}
//			}
//		}
//	}
};

#endif /* 2D_RENDERMANAGER_H_ */
