#ifndef object_h
#define object_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <deque>
#include <vector>

#define JUMP_RATE 30

class Object{
public:
	Object();
	~Object();
	void init(const char *assetFile, int xPos, int yPos, int width, int height, SDL_Renderer *renderer);
	void updatePosition(){};
	void render();
	SDL_Rect rect, srcRect;
	void setSource(int x, int y, int w, int h);
	void updateAnimation();
	void reverse(bool r){rev=r;}
	void reverse(bool r, int nA) {rev=r; nAb=1; nA=newAnim;}

	struct cycle {
		int row;
		int w;
		int h;
		int amount;
		int speed;
		int tick;
	};
	int curAnim;
	int begin;
	int idle_l, idle_r, run_l, run_r;
	bool rev, nAb;
	int newAnim;
	std::vector<cycle> animations;
	int createCycle(int r, int w, int h, int amount, int speed)
	{
		cycle tmp;
		tmp.row = r - 1;
		tmp.w = w;
		tmp.h = h;
		tmp.amount = amount;
		tmp.speed = speed;
		tmp.tick = 0;
		animations.push_back(tmp);
		return animations.size()-1;
	}
	void setCurAnimation(int c) {begin = 0; curAnim = c;}
	

protected:
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	virtual void updatePosition(int screenW, int screenH, int moveRate, const Uint8 *keystate, SDL_Rect *spaceRect1, SDL_Rect *spaceRect2){};
	bool fall;
};


//Obstacle class that builds on the Object class
class Obstacle : public Object{
public:
	using Object::updatePosition;
	virtual void updatePosition(int screenW, int screenH, int moveRate){
		// Move enemy at constant rate
		rect.w = screenW/20;
		rect.h = screenH/10;
		if (rect.x + rect.w > 0){
		    rect.x -= moveRate;
		}
	}
};


//Player class that builds on the Object class
class Player : public Object{
public:
	using Object::updatePosition;
	virtual void updatePosition(int screenW, int screenH, int moveRate, const Uint8* keystate, SDL_Rect *bgRect1, SDL_Rect *bgRect2, SDL_Rect *floorRect, SDL_Rect *floorRect2){
	    rect.w = screenW/8;
	    rect.h = screenH/4;
		
		//move character by keystroke
		//move left
	    if (keystate[SDL_SCANCODE_LEFT] and rect.x >= 0){
	        this->setCurAnimation(run_l);
			rect.x -= moveRate;
			
	    }
		//checks if the character is at the halfway point of the screen width while moving right
	    if (keystate[SDL_SCANCODE_RIGHT] and rect.x + rect.w/2 <= screenW/2){
	        this->setCurAnimation(run_r);
			rect.x += moveRate;
	    }
		//if the character has reached the halfway point of the screen width, the background also moves
		//move right
	    else if (keystate[SDL_SCANCODE_RIGHT]){
			this->setCurAnimation(run_r);
	        bgRect1->x -= moveRate;
	        bgRect2->x -= moveRate;
			floorRect->x -= moveRate;
			floorRect2->x -= moveRate;
			
	        if (bgRect1->x + bgRect1->w <= 0){
	            bgRect1->x = bgRect1->w;
	        }
	        if (bgRect2->x + bgRect2->w <= 0){
	            bgRect2->x = bgRect2->w;
	        }
			if (floorRect->x + floorRect->w <= 0){
	            floorRect->x = floorRect->w;
	        }
			if (floorRect2->x + floorRect2->w <= 0){
	            floorRect2->x = floorRect2->w;
	        }
	    }

		//character jumps
	    if (keystate[SDL_SCANCODE_UP] && rect.y > 100)
		{	
			isJumping = true;
			rect.y -= JUMP_RATE;
	    }

		if(isJumping){
			if(rect.y <= (screenH/4)*3-50){
				if(rect.y <= (screenH/6)*2){
					rect.y += 15;
				}
				else if(rect.y < (screenH/6)*2 && rect.y >= (screenH/6)*5){
					rect.y += 10;
				}
				else{
					rect.y += 8;
				}
			}
			else{
				isJumping = false;
			}
		}

		updateAnimation();
		
		
	}
	void render(){
	SDL_RenderCopyEx(renderer, texture, &srcRect, &rect, 0, NULL, SDL_FLIP_NONE);
	}


protected:
	bool isJumping;
};

#endif