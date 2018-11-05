#define __GAME__VERSION__ "0.1.0"
enum class gameState{menu, inGame, endGame, preGame, pauseMenu}currentState = gameState::menu;
#include "sdl_base.h"
#define settings sdl_settings
#define texture SDL_Texture
#define pii pair<int, int>
#define mp make_pair
#define A first
#define B second
#define PI 3.141592653589793
#define TO_DEG(x) (x*180/PI)
#define TO_RAD(x) (x*PI/180)
using namespace std;
#include <set>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <SDL2/SDL_image.h>
#include "game_functions.h"
#include "menu.h"
#include "pause_menu.h"
#include "gameplay.h"
#include "indiv_control_menu.h"
#include "pre_game.h"
#include "end_game.h"
#include "load_items.h"
int main()
{
    sdl_settings::load_config();
    initSDL();
    loadItems();
    initShopItems();
    initWeapons();
    initPlayers();
    initKeyNames();
    versionInfo::init();
    initMenu();
    while(1)
    {
        switch(currentState)
        {
        case gameState::menu:
            operateMenu();
            break;
        case gameState::preGame:
            operatePreGame();
            break;
        case gameState::inGame:
            operateGame();
            break;
        case gameState::endGame:
            operateEndGame();
            break;
        case gameState::pauseMenu:
            operatePauseMenu();
            break;
        }
    }
    return 0;
}
