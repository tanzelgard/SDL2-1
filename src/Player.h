#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include "World.h"
class Player {
public:
	Player(int x, int y);
	void update(float dt, int winW, int winH, const std::vector<SDL_Rect>& solids, const std::vector<SDL_Rect>& platforms);
	void handleEvent(const SDL_Event& event);
	void draw(SDL_Renderer* renderer) const;
	bool loadTexture(SDL_Renderer* renderer);
	void freeTexture();

private:
	SDL_Rect rect;
	SDL_Rect groundSensor = { 0,0,30,2 };
	bool downHeld = false;
	bool onPlatformOnly = false;
	bool moveLeft = false;
	bool moveRight = false;
	bool isOnGround = false;
	bool facingRight = true;
	bool isSkidding = false;
	float posX, posY;
	float spawnX, spawnY;
	float speed = 300.0f;
	float velocityY = 0.0f;
	float velocityX = 0.0f;
	static constexpr float jumpForce = -675.0f;
	static constexpr float accel = 400.0f;
	static constexpr float skidDecel = 600.0f;
	static constexpr float friction = 500.0f;
	TexturePtr idleTex;
	TexturePtr runTex;
	int runFrames = 3;
	int runFrameH = 0;
	int runFrameW = 0;
	int animFrame = 0;
	float animTimer = 0.0f;
	static constexpr float frameTime = 0.1f;
	TexturePtr jumpTex;
	TexturePtr inertionTex; 
	
};