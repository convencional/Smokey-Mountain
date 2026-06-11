#ifndef AUDIO_H
#define AUDIO_H

#include <SDL3/SDL.h>
#include "engine_data.h"

extern SDL_AudioStream* audio_stream;
extern SomInstancia slotSom;

void GerarSomSFXR(SfxrParams* p);
void DispararSomExemplo(void);

#endif

