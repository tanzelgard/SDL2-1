#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <map>
#include <string>
#include <memory>

struct TextureDeleter {
	void operator()(SDL_Texture* t) const { SDL_DestroyTexture(t); }
};
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
class World {
public:
	bool loadTextures(SDL_Renderer* renderer);
	void freeTextures();
	bool load(const std::string& path);
	void draw(SDL_Renderer* renderer) const;
	const std::vector<SDL_Rect>& getSolids() const;
	const std::vector<SDL_Rect>& getPlatforms() const;

private:
	TexturePtr brick1Tex;
	TexturePtr floorTex;
	TexturePtr platformTex;
	TexturePtr solid1Tex;
	TexturePtr brick2Tex;
	static constexpr int tileSize = 32;
	std::vector<std::string> map;
	std::vector<SDL_Rect> solids;
	std::vector<SDL_Rect> platforms;
};