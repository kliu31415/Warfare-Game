int pause_menu_select;
void operatePauseMenu()
{
    const static int MENU_OPTIONS = 3;
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            quit_game();
            return;
        case SDL_KEYDOWN:
            switch(input.key.keysym.sym)
            {
            case SDLK_UP:
                pause_menu_select += MENU_OPTIONS-1;
                pause_menu_select %= MENU_OPTIONS;
                break;
            case SDLK_DOWN:
                pause_menu_select++;
                pause_menu_select %= MENU_OPTIONS;
                break;
            case SDLK_RETURN:
                switch(pause_menu_select)
                {
                case 0:
                    currentState = gameState::inGame;
                    break;
                case 1:
                    currentState = gameState::menu;
                    break;
                case 2:
                    quit_game();
                    break;
                }
                break;
            }
            break;
        }
    }
    int H = settings::WINDOW_H, W = settings::WINDOW_W;
    renderClear(100, 100, 100);
    drawText("PAUSED", W*0.4, H*0.1, H*0.1);
    fillRect(W*0.39, H*0.19, W*0.22, H*(0.02 + 0.05*MENU_OPTIONS), 150, 150, 150); //outer box
    fillRect(W*0.4, H*0.2, W*0.2, H*0.05*MENU_OPTIONS, 200, 200, 200); //inner box
    fillRect(W*0.4, H*(0.2 + 0.05*pause_menu_select), W*0.2, H*0.05, 200, 150, 100); //highlight the selected option
    setColor(0, 0, 0);
    for(int i=1; i<MENU_OPTIONS; i++)
        drawLine(W*0.4, H*(0.2 + i*0.05), W*0.6, H*(0.2 + i*0.05));
    drawText("RESUME", W*0.4, H*0.2, H*0.05); //option 0
    drawText("MAIN MENU (EXIT)", W*0.4, H*0.25, H*0.05); //option 1
    drawText("QUIT", W*0.4, H*0.3, H*0.05); //option 2
    updateScreen();
}
