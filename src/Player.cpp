#include "Player.h"
#include "Physics.h"
#include "World.h"
#include <iostream>
#include <cmath>

Player::Player(int x, int y) : rect{ x, y, 30, 30 }, posX(x), posY(y), velocityY(0.0f), spawnX(x), spawnY(y) {}

void Player::update(float dt, int winW, int winH, const std::vector<SDL_Rect>& solids, const std::vector<SDL_Rect>& platforms) {
    float dx = 0.0f;
    const int prevBotton = rect.y + rect.h;
    if (moveLeft) dx -= 1.0f;
    if (moveRight) dx += 1.0f;

    if (dx > 0) facingRight = true;
    else if (dx < 0) facingRight = false;

    if (dx != 0.0f) {
        float a = (dx * velocityX < 0.0f) ? skidDecel : accel;
        velocityX += dx * a * dt;
    }
    else {
        float f = friction * dt;
        if (velocityX > f) velocityX -= f;
        else if (velocityX < -f) velocityX += f;
        else velocityX = 0.0f;
    }
    if (velocityX > speed) velocityX = speed;
    if (velocityX < -speed) velocityX = -speed;

    posX += velocityX * dt;
    rect.x = static_cast<int>(posX);
    //solid
    for (const SDL_Rect& s : solids) {
        if (SDL_HasIntersection(&rect, &s)) {
            if (velocityX > 0) {
                posX = s.x - rect.w;
            }
            else if (velocityX < 0) {
                posX = s.x + s.w;
            }
            velocityX = 0.0f;
            rect.x = static_cast<int>(posX);
        }
    }
    //grav
    velocityY += Physics::gravity * dt;
    if (velocityY > Physics::maxFallSpeed) velocityY = Physics::maxFallSpeed;
    posY += velocityY * dt;
    rect.y = static_cast<int>(posY);

    if (posX < 0) {
        posX = 0;
        rect.x = static_cast<int>(posX);
    }
    //solid
    for (const SDL_Rect& s : solids) {
        if (SDL_HasIntersection(&rect, &s))
        {
            if (velocityY > 0) {
                posY = s.y - rect.h;
                velocityY = 0.0f;
            }
            else if (velocityY < 0) {
                posY = s.y + s.h;
                velocityY = 0.0f;
            }
            rect.y = static_cast<int>(posY);
        }

    }
    //platform
    if (velocityY > 0) {
        for (const SDL_Rect& p : platforms) {
            if (SDL_HasIntersection(&rect, &p) && prevBotton <= p.y) {
                posY = p.y - rect.h;
                velocityY = 0.0f;
                rect.y = static_cast<int>(posY);
            }
        }
    }
    if (posY > winH) {
        posX = spawnX;
        posY = spawnY;
        velocityY = 0.0f;
        rect.x = static_cast<int>(posX);
        rect.y = static_cast<int>(posY);
    }
    SDL_Rect probe = rect;
    probe.y += 1;
    isOnGround = false;
    bool onSolid = false;
    //solid
    for (const SDL_Rect& s : solids) {
        if (SDL_HasIntersection(&probe, &s)) { onSolid = true; break; }
    }
    //platform
    bool onPlatform = false;
    if (!isOnGround) {
        for (const SDL_Rect& p : platforms) {
            if (SDL_HasIntersection(&probe, &p) && rect.y + rect.h <= p.y) { onPlatform = true; break; }
        }
    }
    isOnGround = onPlatform || onSolid;
    onPlatformOnly = onPlatform && !onSolid;

    if (posX + rect.w > winW) {
        posX = winW - rect.w;
        rect.x = static_cast<int>(posX);
    }

    if (isOnGround && std::fabs(velocityX) > 10.0f) {
        animTimer += dt;
        if (animTimer >= frameTime) {
            animTimer -= frameTime;
            animFrame = (animFrame + 1) % runFrames;
        }
    }
    else {
        animFrame = 0;
        animTimer = 0.0f;
    }
    isSkidding = isOnGround && (dx * velocityX < 0.0f);
}

void Player::handleEvent(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN && event.type != SDL_KEYUP) return;
    const bool pressed = (event.type == SDL_KEYDOWN);

    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_A: moveLeft = pressed; break;
    case SDL_SCANCODE_D: moveRight = pressed; break;
    case SDL_SCANCODE_S: downHeld = pressed; break;
    case SDL_SCANCODE_SPACE:
        if (pressed && isOnGround && event.key.repeat == 0) {
            if (downHeld && onPlatformOnly) {
                posY += 2.0f;
                rect.y = static_cast<int>(posY);
                isOnGround = false;
            }
            else {
                velocityY = jumpForce;
                isOnGround = false;
            }
        }
        if (!pressed && velocityY < 0.0f) velocityY *= 0.5f;
        break;

    default: break;
    }
}
bool Player::loadTexture(SDL_Renderer* renderer) {
    idleTex.reset(IMG_LoadTexture(renderer, "assets/playerIdle.png"));
    runTex.reset(IMG_LoadTexture(renderer, "assets/playerRunAnim.png"));
    jumpTex.reset(IMG_LoadTexture(renderer, "assets/playerJump.png"));
    inertionTex.reset(IMG_LoadTexture(renderer, "assets/playerInertion.png"));
    const bool pt = idleTex && runTex && jumpTex && inertionTex;
    if (!pt) {
        std::cerr << "Текстуры игрока не загрузились!" << IMG_GetError() << "\n";
        return false;
    }
    int texW, texH;
    SDL_QueryTexture(runTex.get(), nullptr, nullptr, &texW, &texH);
    runFrameW = texW / runFrames;
    runFrameH = texH;
    return true;
}
void Player::freeTexture() {
    idleTex.reset();
    runTex.reset();
    jumpTex.reset();
    inertionTex.reset();
}
void Player::draw(SDL_Renderer* renderer) const{
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    const bool running = isOnGround && std::fabs(velocityX) > 10.0f;
    if (!isOnGround && jumpTex) {
         SDL_RenderCopyEx(renderer, jumpTex.get(), nullptr, &rect, 0.0, nullptr, flip);
    }
    else if (isSkidding && inertionTex) {
        SDL_RenderCopyEx(renderer, inertionTex.get(), nullptr, &rect, 0.0, nullptr, flip);
    }
    else if (running && runTex) {
        SDL_Rect src = { animFrame * runFrameW, 0, runFrameW, runFrameH };
        SDL_RenderCopyEx(renderer, runTex.get(), &src, &rect, 0.0, nullptr, flip);
    }
    else if (idleTex) {
        SDL_RenderCopyEx(renderer, idleTex.get(), nullptr, &rect, 0.0, nullptr, flip);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}
