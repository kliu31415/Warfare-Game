#include "maps.h"
struct pickup;
vector<pickup> pickups;
#include "weapons.h"
//vectors of weapons and projectiles
#include "pickups.h"
struct player;
vector<player> players;
#include "player.h"
int gameTimeLeft; //milliseconds
void processProjectiles();
void initEndGame();
void handleExplosions();
void operateGame()
{
    //first handle input
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            quit_game();
            break;
        case SDL_KEYDOWN:
            for(int i=0; i<NUM_PLAYERS; i++)
            {
                if(input.key.keysym.sym == SDL_GetKeyFromScancode(players[i].pauseGame))
                    currentState = gameState::pauseMenu;
            }
            break;
        }
    }
    //now render everything (and process the players I guess)
    renderClear(0, 0, 0);
    using namespace settings;
    gameMaps[currentMap].render();
    processPickups();
    processProjectiles();
    const uint8_t *keystate = SDL_GetKeyboardState(NULL);
    for(int i=0; i<NUM_PLAYERS; i++)
        players[i].process(keystate);
    handleText(); //render all the various texts across the screen
    for(auto &i: projectiles) //makes high velocity projectiles start closer to the player
        i.render();
    handleExplosions(); //show the explosions on the screen
    fillRect(BLOCK_SIZE*MAP_W, 0, WINDOW_W-BLOCK_SIZE*MAP_W, WINDOW_H, 220, 200, 220); //projectiles shouldn't be appearing on the sidebar
    for(int i=0; i<NUM_PLAYERS; i++)
        players[i].render();
    gameTimeLeft -= getFrameLength();
    int baseFontSize = WINDOW_H/40; //standard font size
    drawText(seconds_to_str((gameTimeLeft+999)/1000), BLOCK_SIZE*MAP_W*0.45, MAP_H*BLOCK_SIZE*0.45, BLOCK_SIZE*(MAP_W*0.55-MAP_W*0.45)/2, 255, 50, 50);
    if(gameTimeLeft <= 0) //the game is over
    {
        initEndGame();
    }
    updateScreen();
}
#include "process_stuff.h"
void initGame(int *gold, set<int> *bought)
{
    pause_menu_select = 0;
    projectiles.clear();
    pickups.clear();
    fill(pickup_spawn_occupied, pickup_spawn_occupied + PICKUP_SPAWN_POINTS, false);
    currentState = gameState::inGame;
    gameTimeLeft = GAME_LENGTH * 1000;
    for(int i=0; i<NUM_PLAYERS; i++)
    {
        players[i].reset();
        players[i].gold = gold[i];
        for(auto &j: bought[i])
            players[i].addShopItem(j);
    }
    disapText.clear();
}
void initPlayers()
{
    for(int i=0; i<MAX_PLAYERS; i++)
    {
        players.push_back({i});
    }
}
