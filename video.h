#ifndef VIDEO_H
#define VIDEO_H

#include <SDL3/SDL.h>
#include "engine_data.h"

extern SDL_Texture* tileset_texture;

void CriarTexturasDoJogo(SDL_Renderer* renderer);
void DesenharGrafico(SDL_Renderer* r, SDL_Texture* t, int32_t idGfx,
    int32_t posX, int32_t posY, int32_t largDest, int32_t altDest,
    int32_t largOrig, int32_t altOrig, int32_t flip);

#endif
