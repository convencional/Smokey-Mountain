#include "video.h"

SDL_Texture* tileset_texture = NULL;

static const Uint32 PaletaHexa[256] = {
    ['E'] = 0xFFFFFF00, // Chão (Amarelo Claro)
    ['2'] = 0xFF00A000, // Paredes (Verde)
    ['1'] = 0xFF0000A0, // Arqueiro (Azul Escuro)
    ['8'] = 0xFF000000  // Flechas (Preto)
};

static const char* DungeonTilesData[6][8] = {
    {"EEEEEEEE","EEEEEEEE","EEEEEEEE","EEEEEEEE","EEEEEEEE","EEEEEEEE","EEEEEEEE","EEEEEEEE"},
    {"22222222","22222222","22222222","22222222","22222222","22222222","22222222","22222222"},
    {"22222222","2222222E","222222EE","22222EEE","2222EEEE","222EEEEE","22EEEEEE","2EEEEEEE"},
    {"22222222","E2222222","EE222222","EEE22222","EEEE2222","EEEEE222","EEEEEE22","EEEEEEE2"},
    {"EEEEEEE2","EEEEEE22","EEEEE222","EEEE2222","EEE22222","EE222222","E2222222","22222222"},
    {"2EEEEEEE","22EEEEEE","222EEEEE","2222EEEE","22222EEE","222222EE","2222222E","22222222"}
};

static const char* Sprites8x8Data[3][8] = {
    {"    8   ","     8  ","      8 ","88888888","      8 ","     8  ","    8   ","        "}, // Flecha H
    {"   8    ","  888   "," 8 8 8  ","   8    ","   8    ","   8    ","   8    ","   8    "}, // Flecha V
    {"    888 ","     88 ","    8 8 ","   8    ","  8     "," 8      ","8       ","        "}  // Flecha D
};

static const char* Sprites8x16Data[4][16] = {
    {"  111   ",
    "  1111  ",
    "      1 ",
    "  11  1 ",
    "  11  1 ",
    "  11  1 ",
    "  11 1  ",
    "  111   ",
    "  11    ",
    "  11    ",
    "  11    ",
    "1111    ",
    "1 1     ",
    "1 1     ",
    "  1     ",
    "  11    "},
    // Frame 1
    {"    11  ","    111 ","       1","  111  1"," 1 111 1"," 1 111 1"," 1 11 1 ","     1  ",
     "  111   "," 111111 "," 1    1 "," 1  111 "," 1  1   ","1   1   ","1       ","1       "},
     // Frame 2
     {"    111 ","    1111"," 111    "," 1 11   "," 1 11111","   11   ","   11  1","      1 ",
      "   11   ","  111111","  1    1"," 11    1"," 1     1","11      ","1       ","        "},
      // Frame 3
      {"     11 ","     111","        ","   111  ","  1111  ","  1 111 ","  1 11 1","      1 ",
       "  111   ","  1111  ","  1  1  ","111  1  ","1    1  ","1    1  ","     1  ","     11 "}
};

void CriarTexturasDoJogo(SDL_Renderer* renderer) {
    SDL_Surface* surface = SDL_CreateSurface(32, 48, SDL_PIXELFORMAT_ARGB8888);
    if (!surface) return;
    SDL_ClearSurface(surface, 0, 0, 0, 0);
    Uint32* pixels = (Uint32*)surface->pixels;
    int pitch_pixels = surface->pitch / 4;

    for (int t = 0; t < 6; t++) {
        int tileX = (t % 4) * 8, tileY = (t / 4) * 8;
        for (int y = 0; y < 8; y++) {
            const unsigned char* src = (const unsigned char*)DungeonTilesData[t][y];
            Uint32* dst = &pixels[(tileY + y) * pitch_pixels + tileX];
            for (int i = 0; i < 8; i++) *dst++ = PaletaHexa[*src++];
        }
    }
    for (int f = 0; f < 3; f++) {
        int id = 6 + f, tileX = (id % 4) * 8, tileY = (id / 4) * 8;
        for (int y = 0; y < 8; y++) {
            const unsigned char* src = (const unsigned char*)Sprites8x8Data[f][y];
            Uint32* dst = &pixels[(tileY + y) * pitch_pixels + tileX];
            for (int i = 0; i < 8; i++) *dst++ = PaletaHexa[*src++];
        }
    }
    for (int f = 0; f < 4; f++) {
        int archX = f * 8, archY = 24;
        for (int y = 0; y < 16; y++) {
            const unsigned char* src = (const unsigned char*)Sprites8x16Data[f][y];
            Uint32* dst = &pixels[(archY + y) * pitch_pixels + archX];
            for (int i = 0; i < 8; i++) *dst++ = PaletaHexa[*src++];
        }
    }
    tileset_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
}

void DesenharGrafico(SDL_Renderer* r, SDL_Texture* t, int32_t idGfx,
    int32_t posX, int32_t posY, int32_t largDest, int32_t altDest,
    int32_t largOrig, int32_t altOrig, int32_t flip) {
    if (posX <= -largDest || posX >= 320 || posY <= -altDest || posY >= 240) return;
    float srcX = (idGfx <= 8) ? (float)((idGfx % 4) * 8) : (float)((idGfx - 9) * 8);
    float srcY = (idGfx <= 8) ? (float)((idGfx / 4) * 8) : 24.0f;

    SDL_FRect srcRect = { srcX, srcY, (float)largOrig, (float)altOrig };
    SDL_FRect dstRect = { (float)posX, (float)posY, (float)largDest, (float)altDest };
    SDL_FlipMode sdlFlip = SDL_FLIP_NONE;
    if (flip & 1) sdlFlip |= SDL_FLIP_HORIZONTAL;
    if (flip & 2) sdlFlip |= SDL_FLIP_VERTICAL;
    SDL_RenderTextureRotated(r, t, &srcRect, &dstRect, 0.0, NULL, sdlFlip);
}
