#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "engine_data.h"
#include "audio.h"
#include "video.h"
#include "dungeon.h"
#include "flechas.h"

int main(int argc, char* argv[]) {
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) return 1;

    SDL_Window* window = SDL_CreateWindow("Cloudy Mountain - Pixel Raw", 640, 480, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderLogicalPresentation(renderer, 320, 240, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    SDL_AudioSpec spec = { SDL_AUDIO_F32, 1, SAMPLE_RATE };
    audio_stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (audio_stream) {
        SDL_ResumeAudioStreamDevice(audio_stream);
    }

    GerarNovaSemente();
    BakeDungeon();
    CriarTexturasDoJogo(renderer);
    SDL_SetTextureScaleMode(tileset_texture, SDL_SCALEMODE_NEAREST);

    int CamX = 4096, CamY = 4096;
    int playerX = 152, playerY = 112;
    int frameAnima = 0, contadorFrame = 0, direcaoPlayer = 0;
    bool running = true;
    SDL_Event event;
    const bool* keys = SDL_GetKeyboardState(NULL);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
        }

        int dx = (keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT]) << 1;
        int dy = (keys[SDL_SCANCODE_DOWN] - keys[SDL_SCANCODE_UP]) << 1;

        int k7 = keys[SDL_SCANCODE_KP_7], k8 = keys[SDL_SCANCODE_KP_8], k9 = keys[SDL_SCANCODE_KP_9];
        int k4 = keys[SDL_SCANCODE_KP_4], k6 = keys[SDL_SCANCODE_KP_6];
        int k1 = keys[SDL_SCANCODE_KP_1], k2 = keys[SDL_SCANCODE_KP_2], k3 = keys[SDL_SCANCODE_KP_3];

        int32_t temTiro = k7 | k8 | k9 | k4 | k6 | k1 | k2 | k3;
        int32_t idxTiro = (k8 * 1) + (k9 * 2) + (k4 * 3) + (k6 * 4) + (k1 * 5) + (k2 * 6) + (k3 * 7);
        int32_t tiroDirecao = (temTiro * TabelaTiroKeypad[idxTiro]) + (!temTiro * -1);

        travaTiro = (travaTiro > 0) ? (travaTiro - 1) : 0;
        if (tiroDirecao != -1 && travaTiro == 0) {
            int32_t playerAbsX = (CamX + playerX + 4) & 4095;
            int32_t playerAbsY = (CamY + playerY + 8) & 4095;
            CriarFlecha(playerAbsX, playerAbsY, tiroDirecao, ATOR_PLAYER);
            DispararSomExemplo();
            travaTiro = 15;
        }

        AtualizarFlechas();
        direcaoPlayer = keys[SDL_SCANCODE_LEFT] ? 1 : (keys[SDL_SCANCODE_RIGHT] ? 0 : direcaoPlayer);

        int absX = (CamX + playerX) & 4095;
        int absY = (CamY + playerY) & 4095;
        int cEsq = 3, cDir = 12, cTopo = 8, cBase = 15;

        int checkX = (CamX + playerX + (dx > 0 ? cDir : cEsq) + dx) & 4095;
        unsigned char colX_topo = CelulaMundo(checkX, (absY + cTopo) & 4095);
        unsigned char colX_base = CelulaMundo(checkX, (absY + cBase) & 4095);
        int moverX = (colX_topo == 0) && (colX_base == 0);
        CamX = (CamX + (dx * moverX)) & 4095;

        int checkY = (CamY + playerY + (dy > 0 ? cBase : cTopo) + dy) & 4095;
        unsigned char colY_esq = CelulaMundo((CamX + playerX + cEsq) & 4095, checkY);
        unsigned char colY_dir = CelulaMundo((CamX + playerX + cDir) & 4095, checkY);
        int moverY = (colY_esq == 0) && (colY_dir == 0);
        CamY = (CamY + (dy * moverY)) & 4095;

        int estaMovendo = (dx != 0) || (dy != 0);
        contadorFrame += estaMovendo;
        int resetAnima = (contadorFrame > 6);
        frameAnima = (frameAnima + resetAnima) & 3;
        contadorFrame = contadorFrame * (!resetAnima) * estaMovendo;
        frameAnima = frameAnima * estaMovendo;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        int baseTileX = CamX >> 4, baseTileY = CamY >> 4;
        int offsetX = CamX & 15, offsetY = CamY & 15;

        for (int tY = 0; tY <= 15; tY++) {
            for (int tX = 0; tX <= 21; tX++) {
                int virtualTileX = (baseTileX + tX) & 63;
                int virtualTileY = (baseTileY + tY) & 63;
                int blocoId = Dungeon[(virtualTileY << 6) + virtualTileX];
                float srcX = (float)((blocoId % 4) * 8);
                float srcY = (float)((blocoId / 4) * 8);
                SDL_FRect srcRect = { srcX, srcY, 8.0f, 8.0f };
                SDL_FRect dstRect = { (float)((tX << 4) - offsetX), (float)((tY << 4) - offsetY), 16.0f, 16.0f };
                SDL_RenderTexture(renderer, tileset_texture, &srcRect, &dstRect);
            }
        }

        int32_t arqueiroIdGfx = 9 + frameAnima;
        DesenharGrafico(renderer, tileset_texture, arqueiroIdGfx, playerX, playerY, 16, 16, 8, 16, direcaoPlayer & 1);
        RenderizarFlechas(renderer, tileset_texture, CamX, CamY);

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if (slotSom.buffer) free(slotSom.buffer);
    SDL_DestroyTexture(tileset_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
