#include "move.hpp"

#include "id.hpp"

namespace Sound {

	static constexpr unsigned char sfx_wav[] = {};
	static constexpr unsigned int sfx_wav_len = 14110;


	static SDL_AudioStream* audioStream;
	static SDL_AudioSpec wav_spec = {};
	static Uint8* wav_buffer = nullptr;
	static Uint32 wav_length = 0;

	static Uint8* adjusted_wav_buffer = nullptr;

	void init_move() {
		SDL_LoadWAV_IO(SDL_IOFromConstMem(sfx_wav, sfx_wav_len), true, &wav_spec, &wav_buffer, &wav_length);

        audioStream = SDL_CreateAudioStream(&wav_spec, &wav_spec);
        SDL_BindAudioStream(audioDeviceId, audioStream);

        adjusted_wav_buffer = (Uint8*)SDL_malloc(wav_length);
        SDL_memset(adjusted_wav_buffer, 0, wav_length);
        SDL_MixAudio(adjusted_wav_buffer, wav_buffer, wav_spec.format, wav_length, 1.0f);
	}

	void play_move() {
		SDL_ClearAudioStream(audioStream);
		SDL_PutAudioStreamData(audioStream, adjusted_wav_buffer, wav_length);
	}

	void set_volume_move(float volume) {
		SDL_memset(adjusted_wav_buffer, 0, wav_length);
        SDL_MixAudio(adjusted_wav_buffer, wav_buffer, wav_spec.format, wav_length, volume);
	}

	void destroy_move() {
		SDL_DestroyAudioStream(audioStream);
		SDL_free(wav_buffer);
		SDL_free(adjusted_wav_buffer);
	}
}