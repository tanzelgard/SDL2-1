#include "World.h"
#include <fstream>
#include <iostream>

static bool isSolid(char c)
{
	return c == '1' || c == '2' || c == '3' || c == '5';
}
static bool isPlatform(char c)
{
	return c == '4';
}
bool World::load(const std::string& path) {
	map.clear();
	solids.clear();
	platforms.clear();

	std::ifstream file(path);
	if (!file) return false;

	std::string line;
	while (std::getline(file, line)) {
		if (!line.empty() && line.back() == '\r') line.pop_back();
		map.push_back(line);
	}
	for (int row = 0; row < static_cast<int>(map.size()); row++) {
		for (int col = 0; col < static_cast<int>(map[row].size()); col++) {
			SDL_Rect tile = { col * tileSize, row * tileSize, tileSize, tileSize };
			if (isSolid(map[row][col])) solids.push_back(tile);
			else if (isPlatform(map[row][col])) platforms.push_back(tile);
		}
	}
	return true;
}
void World::draw(SDL_Renderer* renderer) const {
	for (int row = 0; row < static_cast<int>(map.size()); row++) {
		for (int col = 0; col < static_cast<int>(map[row].size()); col++) {
			char c = map[row][col];
			SDL_Rect tile{ col * tileSize, row * tileSize, tileSize, tileSize };
			if (c == '1') {
				if (floorTex) {
					SDL_RenderCopy(renderer, floorTex.get(), nullptr, &tile);
				}
				else {
					SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
					SDL_RenderFillRect(renderer, &tile);
				}
			}
			if (c == '2') {
				if (brick1Tex) {
					SDL_RenderCopy(renderer, brick1Tex.get(), nullptr, &tile);
				}
				else {
					SDL_SetRenderDrawColor(renderer, 200, 160, 60, 255);
					SDL_RenderFillRect(renderer, &tile);
				}
			}
			if (c == '3') {
				if (solid1Tex) {
					SDL_RenderCopy(renderer, solid1Tex.get(), nullptr, &tile);
				}
				else {
					SDL_SetRenderDrawColor(renderer, 200, 160, 60, 255);
					SDL_RenderFillRect(renderer, &tile);
				}
			}
			if (c == '4') {
				if (platformTex) {
					SDL_RenderCopy(renderer, platformTex.get(), nullptr, &tile);
				}
				else {
					SDL_SetRenderDrawColor(renderer, 200, 160, 60, 255);
					SDL_RenderFillRect(renderer, &tile);
				}
			}
			if (c == '5') {
				if (brick2Tex) {
					SDL_RenderCopy(renderer, brick2Tex.get(), nullptr, &tile);
				}
				else {
					SDL_SetRenderDrawColor(renderer, 200, 160, 60, 255);
					SDL_RenderFillRect(renderer, &tile);
				}
			}
		}
	}
}

const std::vector<SDL_Rect>& World::getSolids() const {
	return solids;
}
const std::vector<SDL_Rect>& World::getPlatforms() const {
	return platforms;
}

bool World::loadTextures(SDL_Renderer* renderer) {
	brick1Tex.reset(IMG_LoadTexture(renderer, "assets/brick1.png"));
	brick2Tex.reset(IMG_LoadTexture(renderer, "assets/brick2.png"));
	floorTex.reset(IMG_LoadTexture(renderer, "assets/floor.png"));
	platformTex.reset(IMG_LoadTexture(renderer, "assets/platform.png"));
	solid1Tex.reset(IMG_LoadTexture(renderer, "assets/solid1.png"));
	const bool wt = brick1Tex && floorTex && platformTex && solid1Tex;
	if (!wt) {
		std::cerr << "Текстуры не загрузились!" << IMG_GetError() << "\n";
		return false;
	}
	return true;
}
void World::freeTextures(){
	brick1Tex.reset();
	brick2Tex.reset();
	platformTex.reset();
	solid1Tex.reset();
	floorTex.reset();

}
