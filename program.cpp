#include "blocks/red.hpp"
#include "blocks/green.hpp"
#include "blocks/blue.hpp"
#include "blocks/orange.hpp"
#include "blocks/purple.hpp"
#include "blocks/yellow.hpp"
#include "blocks/turqoise.hpp"

#include "component/main.hpp"
#include "component/game.hpp"
#include "cursor/cursor.hpp"
#include "logo/logo.hpp"
#include "logo/window.hpp"
#include "name/name.hpp"
#include "settings/settings.hpp"
#include "sound/setup.hpp"
#include "text/f1.hpp"

#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3/SDL.h>

#include <map>
#include <memory>


const char* APP_TITLE = Name::NAME.data();
SDL_Window* window;
SDL_Renderer* renderer;
TTF_TextEngine* textEngine;

std::map<int, std::unique_ptr<Ui::Base>> uiMap;
int UiMode = Ui::Main().id();



void app_render();
void app_mouseup();
void app_mousedown();

void app_switchUi(int id) {
	uiMap.at(UiMode)->close();
	UiMode = id;
	uiMap.at(id)->open();
}

void app_init() {
	if(!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
		SDL_Log("error initializing SDL: %s\n", SDL_GetError());
		return;
	}
	if(!TTF_Init()) {
		SDL_Log("error initializing SDL_ttf: %s\n", SDL_GetError());
		return;
	}
	if(!SDL_CreateWindowAndRenderer(APP_TITLE, 840, 840, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return;
    }
    if(!Sound::init()) {
    	SDL_Log("Couldn't open audio device/stream: %s", SDL_GetError());
    	return;	
    }
    textEngine = TTF_CreateRendererTextEngine(renderer);

    uiMap.emplace(Ui::Main().id(), std::make_unique<Ui::Main>());
    uiMap.emplace(Ui::Game().id(), std::make_unique<Ui::Game>());

    Logo::init_window();
    SDL_SetWindowIcon(window, Logo::windowSurface);
    Logo::destroy_window();

    Logo::init(renderer);
    Sound::init();
    Font1::init();
    Cursor::init();

    Block::Blue::init(renderer);
    Block::Green::init(renderer);
    Block::Orange::init(renderer);
    Block::Purple::init(renderer);
    Block::Red::init(renderer);
    Block::Turqoise::init(renderer);
    Block::Yellow::init(renderer);

    for(auto& pr : uiMap) {
    	pr.second->init(window, renderer, textEngine, app_switchUi);
    }

    Settings::init(renderer, "settings.json");

    app_render();
}

void app_render() {
	SDL_RenderClear(renderer);

	uiMap.at(UiMode)->render();
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderPresent(renderer);
}

void app_eventloop() {
	SDL_Event event;
    bool running = 1;
    while(running) {
    	while(SDL_PollEvent(&event)) {
	    	switch(event.type) {
	    	case SDL_EVENT_QUIT:
	    		running = 0;
	    		break;
	    	case SDL_EVENT_MOUSE_BUTTON_UP:
	    		uiMap.at(UiMode)->mouseup(event.button.button);
	    		break;
	    	case SDL_EVENT_MOUSE_BUTTON_DOWN:
	    		uiMap.at(UiMode)->mousedown(event.button.button);
	    		break;
	    	case SDL_EVENT_KEY_UP:
	    		uiMap.at(UiMode)->keyup(event.key.key);
	    		break;
	    	case SDL_EVENT_KEY_DOWN:
	    		uiMap.at(UiMode)->keydown(event.key.key);
	    		break;
	    	default:
	    		break;
	    	}
	    }
	    app_render();
    }
}

void app_quit() {

	Logo::destroy();
	Sound::destroy();
	Font1::destroy();
	Cursor::destroy();

	Block::Blue::destroy();
    Block::Green::destroy();
    Block::Orange::destroy();
    Block::Purple::destroy();
    Block::Red::destroy();
    Block::Turqoise::destroy();
    Block::Yellow::destroy();

	for(const auto& p : uiMap) {
		p.second->destroy();
	}

	Settings::destroy();


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_DestroyRendererTextEngine(textEngine);
	TTF_Quit();
	SDL_Quit();
}

int main() {
	app_init();
	app_eventloop();
	app_quit();

	return 0;
}