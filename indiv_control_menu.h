const int MAX_SCANCODES = 256;
string keyName[MAX_SCANCODES];
void initKeyNames()
{
    for(int i=0; i<MAX_SCANCODES; i++)
        keyName[i] = SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)i));
}
void operateIndivControlMenu()
{
    static bool waitingForKey = false;
    const static int MENU_OPTIONS = 10;
    static SDL_Scancode *curKey;
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            quit_game();
            return;
        case SDL_KEYDOWN:
            if(waitingForKey) //the key pressed is a player's controls
            {
                waitingForKey = false;
                const uint8_t *keystate = SDL_GetKeyboardState(NULL);
                for(int i=0; i<MAX_SCANCODES; i++)
                {
                    if(keystate[i])
                    {
                        *curKey = (SDL_Scancode)i;
                        break;
                    }
                }
            }
            else
            {
                switch(input.key.keysym.sym)
                {
                case SDLK_UP:
                    menu_select.back() += MENU_OPTIONS-1;
                    menu_select.back() %= MENU_OPTIONS;
                    break;
                case SDLK_DOWN:
                    menu_select.back()++;
                    menu_select.back() %= MENU_OPTIONS;
                    break;
                case SDLK_ESCAPE:
                    prevMenu();
                    break;
                case SDLK_RETURN:
                    const uint8_t *keystate = SDL_GetKeyboardState(NULL);
                    if(keystate[SDL_SCANCODE_RALT] || keystate[SDL_SCANCODE_LALT])
                    {
                        switch(menu_select.back())
                        {
                        case 0:
                            players[indiv_selected].jump = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 1:
                            players[indiv_selected].moveLeft = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 2:
                            players[indiv_selected].moveRight = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 3:
                            players[indiv_selected].swapWeapons = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 4:
                            players[indiv_selected].fireUp = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 5:
                            players[indiv_selected].fireDown = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 6:
                            players[indiv_selected].fireRight = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 7:
                            players[indiv_selected].fireLeft = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 8:
                            players[indiv_selected].pauseGame = SDL_SCANCODE_NOT_ASSIGNED;
                            break;
                        case 9:
                            prevMenu();
                            break;
                        }
                    }
                    else switch(menu_select.back())
                    {
                    case 0:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].jump;
                        break;
                    case 1:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].moveLeft;
                        break;
                    case 2:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].moveRight;
                        break;
                    case 3:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].swapWeapons;
                        break;
                    case 4:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].fireUp;
                        break;
                    case 5:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].fireDown;
                        break;
                    case 6:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].fireRight;
                        break;
                    case 7:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].fireLeft;
                        break;
                    case 8:
                        waitingForKey = true;
                        curKey = &players[indiv_selected].pauseGame;
                        break;
                    case 9:
                        prevMenu();
                        break;
                    }
                    break;
                }
            }
            break;
        }
    }
    int H = settings::WINDOW_H, W = settings::WINDOW_W;
    renderClear(100, 100, 100);
    drawText("PLAYER " + to_str(indiv_selected+1), MENU_W_OFFSET, H*0.1, H*0.1);
    if(waitingForKey)
        drawText("Press A Key...", MENU_W_OFFSET, H*(0.26 + 0.05*MENU_OPTIONS), H*0.05);
    fillRect(MENU_W_OFFSET - W*0.01, H*0.19, MENU_W + W*0.02, H*(0.02 + 0.05*MENU_OPTIONS), 150, 150, 150); //outer box
    fillRect(MENU_W_OFFSET, H*0.2, MENU_W, H*0.05*MENU_OPTIONS, 200, 200, 200); //inner box
    fillRect(MENU_W_OFFSET, H*(0.2 + 0.05*menu_select.back()), MENU_W, H*0.05, 200, 150, 100); //highlight the selected option
    setColor(0, 0, 0);
    for(int i=1; i<MENU_OPTIONS; i++)
        drawLine(MENU_W_OFFSET, H*(0.2 + i*0.05), MENU_W_OFFSET + MENU_W, H*(0.2 + i*0.05));
    drawText("JUMP", MENU_W_OFFSET, H*0.2, H*0.05); //option 0
    drawText(keyName[players[indiv_selected].jump], MENU_W_OFFSET + MENU_W + W*0.02, H*0.2, H*0.05);
    drawText("MOVE LEFT", MENU_W_OFFSET, H*0.25, H*0.05); //option 1
    drawText(keyName[players[indiv_selected].moveLeft], MENU_W_OFFSET + MENU_W + W*0.02, H*0.25, H*0.05);
    drawText("MOVE RIGHT", MENU_W_OFFSET, H*0.3, H*0.05); //option 2
    drawText(keyName[players[indiv_selected].moveRight], MENU_W_OFFSET + MENU_W + W*0.02, H*0.3, H*0.05);
    drawText("SWAP WEAPONS", MENU_W_OFFSET, H*0.35, H*0.05); //option 3
    drawText(keyName[players[indiv_selected].swapWeapons], MENU_W_OFFSET + MENU_W + W*0.02, H*0.35, H*0.05);
    drawText("FIRE UP", MENU_W_OFFSET, H*0.4, H*0.05); //option 4
    drawText(keyName[players[indiv_selected].fireUp], MENU_W_OFFSET + MENU_W + W*0.02, H*0.4, H*0.05);
    drawText("FIRE DOWN", MENU_W_OFFSET, H*0.45, H*0.05); //option 5
    drawText(keyName[players[indiv_selected].fireDown], MENU_W_OFFSET + MENU_W + W*0.02, H*0.45, H*0.05);
    drawText("FIRE RIGHT", MENU_W_OFFSET, H*0.5, H*0.05); //option 6
    drawText(keyName[players[indiv_selected].fireRight], MENU_W_OFFSET + MENU_W + W*0.02, H*0.5, H*0.05);
    drawText("FIRE LEFT", MENU_W_OFFSET, H*0.55, H*0.05); //option 7
    drawText(keyName[players[indiv_selected].fireLeft], MENU_W_OFFSET + MENU_W + W*0.02, H*0.55, H*0.05);
    drawText("PAUSE GAME", MENU_W_OFFSET, H*0.6, H*0.05); //option 8
    drawText(keyName[players[indiv_selected].pauseGame], MENU_W_OFFSET + MENU_W + W*0.02, H*0.6, H*0.05);
    drawText("BACK", MENU_W_OFFSET, H*0.65, H*0.05); //option 9
    string text = "PRESS ALT + ENTER TO RESET A FIELD";
    drawText(text, W*0.5 - H*0.0125*text.size(), H*0.71, H*0.05);
}
