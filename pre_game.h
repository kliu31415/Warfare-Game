namespace pregameVars
{
    bool ready[MAX_PLAYERS];
    const int READY_WAIT_TIME = 10000; //milliseconds from when the first player is ready to when the game automatically starts
    int readyTime;
    int selected[MAX_PLAYERS];
    int gold[MAX_PLAYERS];
    set<int> bought[MAX_PLAYERS];
    inline SDL_Keycode KFS(SDL_Scancode x)
    {
        return SDL_GetKeyFromScancode(x);
    }
}
struct shopItem
{
    texture *t;
    int cost;
    string name;
    vector<string> description;
};
shopItem shopItems[NUM_SHOP_ITEMS];
void initShopItems()
{
    //shop item 0
    shopItems[0].t = loadTexture("S_bootsofagility.png", 255, 255, 255);
    shopItems[0].name = "Boots of Agility";
    shopItems[0].description.push_back("+10% fall resistance");
    shopItems[0].description.push_back("+5% jump velocity");
    shopItems[0].description.push_back("+5% movement speed");
    shopItems[0].cost = 200;
    //shop item 1
    shopItems[1].t = loadTexture("S_flute.png", 255, 255, 255);
    shopItems[1].name = "Mr. J. Szo's Flute";
    shopItems[1].description.push_back("+10% damage");
    shopItems[1].cost = 200;
    //shop item 2
    shopItems[2].t = loadTexture("S_suspicioussack.png", 255, 255, 255);
    shopItems[2].name = "Suspicious Sack";
    shopItems[2].description.push_back("+50% gold from chests");
    shopItems[2].cost = 250;
    //shop item 3
    shopItems[3].t = loadTexture("S_trumpet.png", 255, 255, 255);
    shopItems[3].name = "Mr. J. Lee's Trumpet";
    shopItems[3].description.push_back("-30% damage decay rate");
    shopItems[3].description.push_back("+20% projectile lifespan");
    shopItems[3].cost = 100;
    //shop item 4
    shopItems[4].t = loadTexture("S_jacket.png", 255, 255, 255);
    shopItems[4].name = "Mr. N. W. S.'s Jacket";
    shopItems[4].description.push_back("+50 max health");
    shopItems[4].description.push_back("Regenerate 5 health/second");
    shopItems[4].description.push_back("-10% explosion damage received");
    shopItems[4].cost = 240;
    //shop item 5
    shopItems[5].t = loadTexture("S_calculator.png", 255, 255, 255);
    shopItems[5].name = "TI-84.5";
    shopItems[5].description.push_back("Fire 10% faster");
    shopItems[5].cost = 180;
    //shop item 6
    shopItems[6].t = loadTexture("S_exeknife.png", 255, 255, 255);
    shopItems[6].name = "Executioner's Knife";
    shopItems[6].description.push_back("+2% damage per kill");
    shopItems[6].cost = 210;
    //shop item 7
    shopItems[7].t = loadTexture("S_sadistsmachete.png", 255, 255, 255);
    shopItems[7].name = "Sadist's Machete";
    shopItems[7].description.push_back("+10% lifesteal");
    shopItems[7].cost = 220;
    //shop item 8
    shopItems[8].t = loadTexture("S_mathexam.png", 255, 255, 255);
    shopItems[8].name = "Mr. J.H.L.'s Math Exam";
    shopItems[8].description.push_back("+20% explosion damage dealt");
    shopItems[8].cost = 90;
    //shop item 9
    shopItems[9].t = loadTexture("S_unidentifiedflask.png", 255, 255, 255);
    shopItems[9].name = "Unidentified Lab Flask";
    shopItems[9].description.push_back("+10% explosion size");
    shopItems[9].description.push_back("+7% damage");
    shopItems[9].description.push_back("+5% lifesteal");
    shopItems[9].description.push_back("Lose 5HP/second");
    shopItems[9].cost = 260;
}
void initPregame()
{
    using namespace pregameVars;
    initMaps();
    currentState = gameState::preGame;
    readyTime = -1;
    fill(ready, ready+NUM_PLAYERS, false);
    fill(selected, selected+NUM_PLAYERS, 0);
    fill(gold, gold+NUM_PLAYERS, STARTING_GOLD);
    for(int i=0; i<NUM_PLAYERS; i++)
        bought[i].clear();
}
void operatePreGame()
{
    using namespace pregameVars;
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            quit_game();
            break;
        case SDL_KEYDOWN:
            SDL_Keycode key = input.key.keysym.sym;
            for(int i=0; i<NUM_PLAYERS; i++)
            {
                player &p = players[i];
                if(key == KFS(p.swapWeapons))
                {
                    ready[i] = true;
                    if(readyTime == -1)
                        readyTime = getTicks() + READY_WAIT_TIME;
                }
                else if(key == KFS(p.jump))
                {
                    if(bought[i].count(selected[i]))
                    {
                        gold[i] += shopItems[selected[i]].cost;
                        bought[i].erase(selected[i]);
                    }
                    else
                    {
                        if(gold[i] >= shopItems[selected[i]].cost)
                        {
                            gold[i] -= shopItems[selected[i]].cost;
                            bought[i].insert(selected[i]);
                        }
                    }
                }
                else if(key == KFS(p.fireLeft))
                {
                    selected[i]--;
                    if(selected[i] < 0)
                        selected[i] += NUM_SHOP_ITEMS;
                }
                else if(key == KFS(p.fireRight))
                {
                    selected[i]++;
                    selected[i] %= NUM_SHOP_ITEMS;
                }
            }
        }
    }
    int W = settings::WINDOW_W, H = settings::WINDOW_H;
    renderClear(0, 0, 0);
    fillRect(BLOCK_SIZE*MAP_W, 0, W-BLOCK_SIZE*MAP_W, H, 220, 200, 220);
    gameMaps[currentMap].render();
    for(int i=0; i<NUM_PLAYERS; i++)
    {
        int hOffset = MAP_W*BLOCK_SIZE; //horizontal offset in pixels of the sidebar
        int pxWidth = W - hOffset; //width of the sidebar in pixels
        int vOffset = i * H/4; //vertical offset in pixels of this player's sidebar
        int baseFontSize = H/40;
        fillRect(hOffset, vOffset - H/100, pxWidth, H/100, 50, 50, 50);
        int ITEM_SIZE = pxWidth/10; //size of an item
        renderCopy(t_gold_icon, hOffset, vOffset, baseFontSize, baseFontSize);
        drawText(to_str(gold[i]), hOffset + baseFontSize, vOffset, baseFontSize);
        if(ready[i])
            drawText(to_str("READY"), hOffset + baseFontSize*4, vOffset, baseFontSize, 255, 0, 0);
        if(readyTime != -1) //some player is ready
        {
            drawText(seconds_to_str((readyTime - getTicks() + 999) / 1000) + " until start", hOffset + baseFontSize*8, vOffset, baseFontSize, 0, 255, 0);
        }
        for(int j=0; j<NUM_SHOP_ITEMS; j++)
            renderCopy(shopItems[j].t, hOffset + ITEM_SIZE*j, vOffset+baseFontSize, ITEM_SIZE, ITEM_SIZE);
        fillRect(hOffset + ITEM_SIZE*selected[i], vOffset+baseFontSize, ITEM_SIZE, ITEM_SIZE, 0, 0, 0, 50);
        shopItem &s = shopItems[selected[i]];
        int curV = vOffset + baseFontSize + ITEM_SIZE; //current vertical offset for text
        drawText(s.name + " (" + to_str(s.cost) + "g)", hOffset, curV, baseFontSize);
        curV += baseFontSize;
        for(auto &j: s.description)
        {
            drawText(j, hOffset, curV, baseFontSize);
            curV += baseFontSize;
        }
        //outline all owned items in green
        for(auto &j: bought[i])
        {
            drawRect(hOffset+j*ITEM_SIZE, vOffset+baseFontSize, ITEM_SIZE, ITEM_SIZE, 0, 255, 0);
        }
    }
    updateScreen();
    if(accumulate(ready, ready + NUM_PLAYERS, 0u) == NUM_PLAYERS) //every player is ready - start the game
        initGame(gold, bought);
    else if(readyTime != -1) //some player is ready, but others are not
    {
        int cur = getTicks();
        if(cur >= readyTime) //time's up!
            initGame(gold, bought);
    }
}
void displayPlayerShopItems(int _p)
{
    using namespace settings;
    player &p = players[_p];
    int hOffset = MAP_W*BLOCK_SIZE; //horizontal offset in pixels of the sidebar
    int vOffset = _p * WINDOW_H/4; //vertical offset in pixels of this player's sidebar
    int baseFontSize = WINDOW_H/40; //standard font size
    int cnt = 0;
    for(auto &i: p.shopItemsBought)
    {
        renderCopy(shopItems[i].t, hOffset + baseFontSize*(2*cnt+10), vOffset + WINDOW_H/36+baseFontSize*4, baseFontSize*2, baseFontSize*2);
        cnt++;
        if(cnt == 3)
        {
            vOffset += baseFontSize*2;
            cnt -= 3;
        }
    }
}
