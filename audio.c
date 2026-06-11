#include "audio.h"
#include <stdlib.h>
#include <math.h>

SDL_AudioStream* audio_stream = NULL;
SomInstancia slotSom = { NULL, 0, 0, false };

static float Frand(void) {
    return (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
}

void GerarSomSFXR(SfxrParams* p) {
    if (slotSom.buffer) { free(slotSom.buffer); slotSom.buffer = NULL; }

    float fperiod = 100.0f / (p->p_base_freq * p->p_base_freq * 9.0f + 0.001f);
    float max_period = 100.0f / (p->p_freq_limit * p->p_freq_limit * 9.0f + 0.001f);
    float fslide = 1.0f - powf(p->p_freq_ramp, 3.0f) * 0.01f;
    float fdslide = -powf(p->p_freq_dramp, 3.0f) * 0.000001f;
    float square_duty = 0.5f - p->p_duty * 0.5f;
    float duty_ramp = -p->p_duty_ramp * 0.00005f;

    int t_attack = (int)(p->p_env_attack * p->p_env_attack * 100000.0f);
    int t_sustain = (int)(p->p_env_sustain * p->p_env_sustain * 100000.0f);
    int t_decay = (int)(p->p_env_decay * p->p_env_decay * 100000.0f);

    int total_samples = t_attack + t_sustain + t_decay;
    if (total_samples <= 0) total_samples = 44100 * 0.2f;

    float* pcm_out = (float*)malloc(total_samples * sizeof(float));
    if (!pcm_out) return;

    int env_stage = 0, env_time = 0;
    float env_vol = 0.0f, phase = 0.0f;
    float noise_buffer[32];
    for (int i = 0; i < 32; i++) noise_buffer[i] = Frand();

    float flpf = powf(p->p_lpf_freq, 3.0f) * 0.1f;
    float flpfd = 1.0f + p->p_lpf_ramp * 0.0001f;
    float flpfr = 1.0f - powf(p->p_lpf_resonance, 2.0f) * 0.1f;
    float flpfv = 0.0f, flpfdv = 0.0f;
    float fhpf = powf(p->p_hpf_freq, 2.0f) * 0.1f;
    float fhpfv = 0.0f;

    for (int i = 0; i < total_samples; i++) {
        env_time++;
        if (env_stage == 0 && env_time >= t_attack) { env_stage = 1; env_time = 0; }
        else if (env_stage == 1 && env_time >= t_sustain) { env_stage = 2; env_time = 0; }

        if (env_stage == 0) env_vol = t_attack > 0 ? (float)env_time / t_attack : 1.0f;
        if (env_stage == 1) env_vol = 1.0f + (1.0f - (t_sustain > 0 ? (float)env_time / t_sustain : 1.0f)) * 2.0f * p->p_env_punch;
        if (env_stage == 2) env_vol = t_decay > 0 ? 1.0f - (float)env_time / t_decay : 0.0f;

        fperiod *= fslide; fslide += fdslide;
        if (fperiod > max_period) fperiod = max_period;
        float period = fperiod;

        if (p->p_vib_strength != 0.0f) {
            float vib_phase = (float)i * p->p_vib_speed * p->p_vib_speed * 0.05f;
            period = fperiod * (1.0f + sinf(vib_phase) * p->p_vib_strength * 0.5f);
        }
        if (period < 1.0f) period = 1.0f;

        square_duty += duty_ramp;
        if (square_duty < 0.0f) square_duty = 0.0f;
        if (square_duty > 0.5f) square_duty = 0.5f;

        phase += 1.0f / period;
        if (phase >= 1.0f) {
            phase -= 1.0f;
            if (p->wave_type == 3) { for (int n = 0; n < 32; n++) noise_buffer[n] = Frand(); }
        }

        float sample = 0.0f;
        switch (p->wave_type) {
        case 0: sample = (phase < square_duty) ? 0.5f : -0.5f; break;
        case 1: sample = 1.0f - phase * 2.0f; break;
        case 2: sample = sinf(phase * 2.0f * 3.14159265f); break;
        case 3: sample = noise_buffer[(int)(phase * 32.0f) & 31]; break;
        }

        float old_flpfv = flpfv;
        flpf *= flpfd;
        if (flpf < 0.0f) flpf = 0.0f; if (flpf > 0.1f) flpf = 0.1f;
        if (p->p_lpf_freq < 1.0f) { flpfdv += (sample - flpfv) * flpf; flpfdv *= flpfr; }
        else { flpfv = sample; flpfdv = 0.0f; }
        flpfv += flpfdv;
        fhpfv += flpfv - old_flpfv; fhpfv *= (1.0f - fhpf);

        float out = fhpfv * env_vol * p->sound_vol * 2.0f;
        if (out > 1.0f) out = 1.0f; if (out < -1.0f) out = -1.0f;
        pcm_out[i] = out;
    }
    slotSom.buffer = pcm_out;
    slotSom.length = total_samples;
    slotSom.play_ptr = 0;
    slotSom.ativo = true;
}

void DispararSomExemplo(void) {
    if (!audio_stream) return;
    SfxrParams tiro = {
        .wave_type = 0, .p_env_attack = 0, .p_env_sustain = 0.277139f,
        .p_env_punch = 0.281669f, .p_env_decay = 0.364357f, .p_base_freq = 0.809276f,
        .p_freq_limit = 0.355084f, .p_freq_ramp = -0.334039f, .p_duty = 0.097348f,
        .p_duty_ramp = 0.032519f, .p_lpf_freq = 1, .p_hpf_freq = 0.115469f, .sound_vol = 0.25f
    };
    SDL_ClearAudioStream(audio_stream);
    GerarSomSFXR(&tiro);
    if (slotSom.buffer && slotSom.length > 0) {
        SDL_PutAudioStreamData(audio_stream, slotSom.buffer, slotSom.length * sizeof(float));
        free(slotSom.buffer);
        slotSom.buffer = NULL;
    }
}
