#include "object.h"

Object::Object(){}
Object::~Object(){}

// initialize object
void Object::init(const char *assetFile, int xPos, int yPos, int width, int height, SDL_Renderer *renderer){
    this->renderer = renderer;
    
    rect.x = xPos;
    rect.y = yPos;
    rect.w = width;
    rect.h = height;

    SDL_Surface *surface = IMG_Load(assetFile);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

// render object
void Object::render(){
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

//set the object source for sprite animations
void Object::setSource(int x, int y, int w, int h){
    srcRect.x=x;
    srcRect.y=y;
    srcRect.w=w;
    srcRect.h=h;
}

//update the animation by changing the source
void Object::updateAnimation(){
    setSource(animations[curAnim].w*animations[curAnim].tick, animations[curAnim].row*animations[curAnim].h, animations[curAnim].w, animations[curAnim].h);
    if(begin>animations[curAnim].speed){
        if(!rev)animations[curAnim].tick++;
        if(rev)animations[curAnim].tick--;
        begin=0;
    }
    begin++;
    if(animations[curAnim].tick >= animations[curAnim].amount) {animations[curAnim].tick=0;}
    if(animations[curAnim].tick <= 0 ) {
        if(nAb) {
            curAnim=newAnim;
            nAb=0;
            rev=0;
        } else {
            animations[curAnim].tick=0;
        }
    }
}