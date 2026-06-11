#include "flechas.h"
#include "dungeon.h"
#include "video.h"

int32_t travaTiro = 0;
static Flecha listaFlechas[MAX_FLECHAS] = { 0 };

const int32_t TabelaTiroKeypad[8] = { 4, 2, 5, 0, 1, 6, 3, 7 };

void CriarFlecha(int32_t origAbsX, int32_t origAbsY, int32_t tiroDirecao, int32_t dono) {
    if (tiroDirecao == -1) return;
    for (int f = 0; f < MAX_FLECHAS; f++) {
        if (listaFlechas[f].ativa == 0) {
            listaFlechas[f].ativa = 1;
            listaFlechas[f].dono = dono;
            listaFlechas[f].absX = origAbsX & 4095;
            listaFlechas[f].absY = origAbsY & 4095;

            int32_t b0 = tiroDirecao & 1;
            int32_t b1 = (tiroDirecao >> 1) & 1;
            int32_t b2 = (tiroDirecao >> 2) & 1;

            listaFlechas[f].idGfx = 6 + b1 + ((b2 << 1) - (b1 * b2));
            int32_t flipH = (b2 == 0) ? (tiroDirecao == 0) : (b0 ^ 1);
            int32_t flipV = (b2 == 0) ? ((tiroDirecao == 3) << 1) : (b1 << 1);
            listaFlechas[f].flipGfx = flipH | flipV;

            int32_t velOrtX = ((tiroDirecao == 1) * 4) - ((tiroDirecao == 0) * 4);
            int32_t velOrtY = ((tiroDirecao == 3) * 4) - ((tiroDirecao == 2) * 4);
            int32_t velDiagX = (b0 * 6) - 3;
            int32_t velDiagY = (b1 * 6) - 3;

            listaFlechas[f].velX = (b2 * velDiagX) + ((1 - b2) * velOrtX);
            listaFlechas[f].velY = (b2 * velDiagY) + ((1 - b2) * velOrtY);
            break;
        }
    }
}

void AtualizarFlechas(void) {
    for (int f = 0; f < MAX_FLECHAS; f++) {
        if (!listaFlechas[f].ativa) continue;
        int32_t proxFX = (listaFlechas[f].absX + listaFlechas[f].velX) & 4095;
        int32_t proxFY = (listaFlechas[f].absY + listaFlechas[f].velY) & 4095;
        int32_t tileAtingido = CelulaMundo(proxFX, proxFY);

        if (tileAtingido > 0) {
            int32_t ehDiag = (listaFlechas[f].velX != 0 && listaFlechas[f].velY != 0) & 1;
            if (ehDiag == 0) {
                int32_t ehQuina = (tileAtingido >= 2);
                int32_t b0 = (tileAtingido - 2) & 1;
                int32_t vX = listaFlechas[f].velX;
                int32_t vY = listaFlechas[f].velY;
                listaFlechas[f].velX = (ehQuina * (vY * ((b0 * 2) - 1))) - ((1 - ehQuina) * vX);
                listaFlechas[f].velY = (ehQuina * (vX * ((b0 * 2) - 1))) - ((1 - ehQuina) * vY);
            }
            else {
                int32_t colX = (CelulaMundo(proxFX, listaFlechas[f].absY) > 0) & 1;
                int32_t colY = (CelulaMundo(listaFlechas[f].absX, proxFY) > 0) & 1;
                int32_t ehQuinaParede = (tileAtingido >= 2) & 1;
                int32_t colVisualQuina = (colX == 0 && colY == 0 && tileAtingido == 1) & 1;
                int32_t multX = (((colX | colVisualQuina | ehQuinaParede) * -2) + 1);
                int32_t multY = (((colY | colVisualQuina | ehQuinaParede) * -2) + 1);
                listaFlechas[f].velX *= multX;
                listaFlechas[f].velY *= multY;
                int32_t erroCego = (colX == 0 && colY == 0 && tileAtingido == 0) & 1;
                listaFlechas[f].velX *= ((erroCego * -2) + 1);
                listaFlechas[f].velY *= ((erroCego * -2) + 1);
            }
            ehDiag = (listaFlechas[f].velX != 0 && listaFlechas[f].velY != 0) & 1;
            int32_t idBase = 7 + ((listaFlechas[f].velX != 0) & 1) * -1;
            listaFlechas[f].idGfx = (ehDiag * 8) + ((1 - ehDiag) * idBase);
            int32_t bitH = (listaFlechas[f].velX < 0) & 1;
            int32_t bitV = ((listaFlechas[f].velY > 0) & 1) << 1;
            int32_t confVertical = (listaFlechas[f].idGfx == 7) & 1;
            bitV = (confVertical * (((listaFlechas[f].velY > 0) & 1) << 1)) + ((1 - confVertical) * bitV);
            listaFlechas[f].flipGfx = bitH | bitV;
        }
        listaFlechas[f].absX = (listaFlechas[f].absX + listaFlechas[f].velX) & 4095;
        listaFlechas[f].absY = (listaFlechas[f].absY + listaFlechas[f].velY) & 4095;
    }
}

void RenderizarFlechas(SDL_Renderer* r, SDL_Texture* t, int32_t atualCamX, int32_t atualCamY) {
    for (int f = 0; f < MAX_FLECHAS; f++) {
        if (!listaFlechas[f].ativa) continue;
        int32_t fX_Tela = listaFlechas[f].absX - atualCamX;
        int32_t fY_Tela = listaFlechas[f].absY - atualCamY;
        if (fX_Tela > 160) fX_Tela -= 4096;
        if (fX_Tela < -160) fX_Tela += 4096;
        if (fY_Tela > 120) fY_Tela -= 4096;
        if (fY_Tela < -120) fY_Tela += 4096;
        DesenharGrafico(r, t, listaFlechas[f].idGfx, fX_Tela, fY_Tela, 8, 8, 8, 8, listaFlechas[f].flipGfx);
    }
}
