/*
 * State.h
 *
 *  Created on: Aug 19, 2016
 *      Author: Niels
 */

#ifndef STATE_H_
#define STATE_H_

class State{
public:
	State(){}
	virtual ~State(){}
	virtual void draw(){}
	virtual void update(double deltaTime){}
	virtual void keypress(unsigned char ch){}
	virtual void keynotpress(unsigned char ch){}
	virtual void click(double percX, double percY, int button){}
};

#endif /* CORE_STATE_H_ */
