#ifndef ENGINE_DATA_H
#define ENGINE_DATA_H

#include <stdint.h>
#include <stdbool.h>

#define SAMPLE_RATE 44100
#define MAP_SIZE 64
#define TOTAL_CELLS (MAP_SIZE * MAP_SIZE)
#define MAX_FLECHAS 8
#define ATOR_PLAYER 0
#define ATOR_INIMIGO 1

typedef struct {
    int wave_type;
    float p_env_attack, p_env_sustain, p_env_punch, p_env_decay;
    float p_base_freq, p_freq_limit, p_freq_ramp, p_freq_dramp;
    float p_vib_strength, p_vib_speed;
    float p_arp_mod, p_arp_speed;
    float p_duty, p_duty_ramp;
    float p_repeat_speed;
    float p_pha_offset, p_pha_ramp;
    float p_lpf_freq, p_lpf_ramp, p_lpf_resonance;
    float p_hpf_freq, p_hpf_ramp;
    float sound_vol;
} SfxrParams;

typedef struct {
    float* buffer;
    int32_t length;
    int32_t play_ptr;
    bool ativo;
} SomInstancia;

typedef struct {
    int32_t ativa;
    int32_t absX;
    int32_t absY;
    int32_t velX;
    int32_t velY;
    int32_t idGfx;
    int32_t flipGfx;
    int32_t dono;
} Flecha;

#endif
