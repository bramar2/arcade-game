#include "theme.hpp"

#include "id.hpp"

namespace Sound {
	static constexpr unsigned int T_wav_len = 14782562;
	static constexpr unsigned char T_wav[] = {};

	static SDL_AudioStream* audioStream;
	static SDL_AudioSpec wav_spec = {};
	static Uint8* wav_buffer = nullptr;
	static Uint32 wav_length = 0;

	static Uint8* adjusted_wav_buffer = nullptr;
	bool on = false;
	void init_theme() {
		SDL_LoadWAV_IO(SDL_IOFromConstMem(T_wav, T_wav_len), true, &wav_spec, &wav_buffer, &wav_length);

        audioStream = SDL_CreateAudioStream(&wav_spec, &wav_spec);
        SDL_BindAudioStream(audioDeviceId, audioStream);

        adjusted_wav_buffer = (Uint8*)SDL_malloc(wav_length);
        SDL_memset(adjusted_wav_buffer, 0, wav_length);
        SDL_MixAudio(adjusted_wav_buffer, wav_buffer, wav_spec.format, wav_length, 1.0f);
	}
    void start_theme() {
        on = true;
    }
    void tick_theme() {
        if(on && SDL_GetAudioStreamQueued(audioStream) < (int)wav_length) {
            SDL_PutAudioStreamData(audioStream, adjusted_wav_buffer, wav_length);
        }
    }
    void stop_theme() {
        on = false;
        SDL_ClearAudioStream(audioStream);
    }
    void set_volume_theme(float volume) {
		SDL_memset(adjusted_wav_buffer, 0, wav_length);
        SDL_MixAudio(adjusted_wav_buffer, wav_buffer, wav_spec.format, wav_length, volume);
	}
	void destroy_theme() {
        SDL_DestroyAudioStream(audioStream);
		SDL_free(wav_buffer);
		SDL_free(adjusted_wav_buffer);
	}
}