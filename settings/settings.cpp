#include "settings.hpp"

#include "icon.hpp"
#include "../sound/blockout.hpp"
#include "../sound/clear_four.hpp"
#include "../sound/hard_drop.hpp"
#include "../sound/line_clear.hpp"
#include "../sound/lock.hpp"
#include "../sound/move.hpp"
#include "../sound/rotate.hpp"
#include "../sound/theme.hpp"

#include "../json/input.hpp"
#include "../json/parse.hpp"
#include "../json/json_bool.hpp"
#include "../json/json_element.hpp"
#include "../json/json_number.hpp"
#include "../json/json_object.hpp"
#include "../json/json_array.hpp"
#include "../json/json_type.hpp"

#include <SDL3/SDL_render.h>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Settings {

	static SDL_Renderer* renderer;
	static std::string file;

	bool vsync = false;
	bool holdAbility = false;
	bool ghostPiece = false;
	int nextPieceCnt = 0;
	int musicVolume = 0;
	int sfxVolume = 0;

	std::vector<unsigned long long> highScores = {};

	void reset() {
  		vsync = false;
		holdAbility = true;
		ghostPiece = true;
		nextPieceCnt = 2;
		musicVolume = 100;
		sfxVolume = 100;
  	}

	void init(SDL_Renderer* i_renderer, const std::string& settingFile) {
		renderer = i_renderer;
		file = settingFile;

		init_icon(renderer);

		reset();
		if(std::filesystem::exists(settingFile)) {
			std::ifstream fin(file);
			json::JsonInput in(&fin);
			std::unique_ptr<json::JsonElement> json = json::parse(in);

			if(json && json->type() == json::JsonType::Object) {
				json::JsonObject* obj = static_cast<json::JsonObject*>(json.get());

				if(obj->contains("vsync") && obj->at("vsync") != nullptr
					&& obj->at("vsync")->type() == json::JsonType::Bool) {
					vsync = static_cast<json::JsonBool*>(obj->at("vsync"))->value;
				}

				if(obj->contains("hold_ability") && obj->at("hold_ability") != nullptr
					&& obj->at("hold_ability")->type() == json::JsonType::Bool) {
					holdAbility = static_cast<json::JsonBool*>(obj->at("hold_ability"))->value;
				}

				if(obj->contains("show_ghost_piece") && obj->at("show_ghost_piece") != nullptr
					&& obj->at("show_ghost_piece")->type() == json::JsonType::Bool) {
					ghostPiece = static_cast<json::JsonBool*>(obj->at("show_ghost_piece"))->value;
				}

				if(obj->contains("next_piece_show_count") && obj->at("next_piece_show_count") != nullptr
					&& obj->at("next_piece_show_count")->type() == json::JsonType::Number) {
					int val = *static_cast<json::JsonNumber*>(obj->at("next_piece_show_count"));
					if(0 <= val && val <= 5) {
						nextPieceCnt = val;
					}
				}

				if(obj->contains("music_volume") && obj->at("music_volume") != nullptr
					&& obj->at("music_volume")->type() == json::JsonType::Number) {
					int val = *static_cast<json::JsonNumber*>(obj->at("music_volume"));
					if(0 <= val && val <= 100) {
						musicVolume = val;
					}
				}

				if(obj->contains("sfx_volume") && obj->at("sfx_volume") != nullptr
					&& obj->at("sfx_volume")->type() == json::JsonType::Number) {
					int val = *static_cast<json::JsonNumber*>(obj->at("sfx_volume"));
					if(0 <= val && val <= 100) {
						sfxVolume = val;
					}
				}
				if(obj->contains("high_score") && obj->at("high_score") != nullptr
					&& obj->at("high_score")->type() == json::JsonType::Array) {
					for(const auto& element : static_cast<json::JsonArray*>(obj->at("high_score"))->elements) {
						if(element == nullptr || element->type() != json::JsonType::Number) continue;
						unsigned long long value = *static_cast<json::JsonNumber*>(element.get());
						highScores.push_back(value);
					}
					std::sort(highScores.rbegin(), highScores.rend());
					while((int)highScores.size() > 5)
						highScores.pop_back();
				}
			}
		}

		save();
	}
	void update() {
		// try adaptive vsync if hardware is available, otherwise normal vsync
		if(!SDL_SetRenderVSync(renderer, vsync ? SDL_RENDERER_VSYNC_ADAPTIVE : SDL_RENDERER_VSYNC_DISABLED)) {
			SDL_SetRenderVSync(renderer, vsync ? 1 : SDL_RENDERER_VSYNC_DISABLED);
		}

		Sound::set_volume_theme(musicVolume / 100.0f);

		Sound::set_volume_blockout(sfxVolume / 100.0f);
		Sound::set_volume_clear_four(sfxVolume / 100.0f);
		Sound::set_volume_hard_drop(sfxVolume / 100.0f);
		Sound::set_volume_line_clear(sfxVolume / 100.0f);
		Sound::set_volume_lock(sfxVolume / 100.0f);
		Sound::set_volume_move(sfxVolume / 100.0f);
		Sound::set_volume_rotate(sfxVolume / 100.0f);
	}

	std::unique_ptr<json::JsonNumber> json_num(unsigned long long value) {
		std::stringstream stream(std::to_string(value));
		json::JsonInput in(&stream);

		auto number = std::make_unique<json::JsonNumber>();
		number->read(in);
		return number;
	}

  	void save() {
  		json::JsonObject json;

  		std::unique_ptr<json::JsonArray> jsonHighScore = std::make_unique<json::JsonArray>();

  		for(unsigned long long score : highScores) {
  			jsonHighScore->elements.push_back(json_num(score));
  		}


  		json.set("vsync", std::make_unique<json::JsonBool>(vsync));
  		json.set("hold_ability", std::make_unique<json::JsonBool>(holdAbility));
  		json.set("show_ghost_piece", std::make_unique<json::JsonBool>(ghostPiece));
  		json.set("next_piece_show_count", json_num(nextPieceCnt));
  		json.set("music_volume", json_num(musicVolume));
  		json.set("sfx_volume", json_num(sfxVolume));
  		json.set("high_score", std::move(jsonHighScore));


  		std::ofstream out(file);
  		json.write(out);
  	}

  	void destroy() {
  		destroy_icon();
  	}
}