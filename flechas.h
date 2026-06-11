#ifndef FLECHAS_H
#define FLECHAS_H

#include <SDL3/SDL.h>
#include "engine_data.h"

extern int32_t travaTiro;
extern const int32_t TabelaTiroKeypad[8];

void CriarFlecha(int32_t origAbsX, int32_t origAbsY, int32_t tiroDirecao, int32_t dono);
void AtualizarFlechas(void);
void RenderizarFlechas(SDL_Renderer* r, SDL_Texture* t, int32_t atualCamX, int32_t atualCamY);

#endif
