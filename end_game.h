namespace endGameVars
{
    const SDL_Keycode goBackToMenu = SDLK_SPACE;
    int winner;
    int laggTime = 1000; //if someone's using goBackToMenu as one of their controls, give them a little time to get their hands off the spacebar
}
void initEndGame()
{
    using namespace endGameVars;
    currentState = gameState::endGame;
    winner = 0;
    for(int i=1; i<NUM_PLAYERS; i++)
    {
        if(players[i].getScore() > players[winner].getScore())
            winner = i;
    }
}
void operateEndGame()
{
    using namespace endGameVars;
    laggTime -= getFrameLength();
    while(SDL_PollEvent(&input))
    {
        switch(input.type)
        {
        case SDL_QUIT:
            quit_game();
            break;
        case SDL_KEYDOWN:
            switch(input.key.keysym.sym)
            {
            case goBackToMenu:
                if(laggTime <= 0)
                    initMenu();
                break;
            }
        }
    }
    int H = settings::WINDOW_H;
    int W = settings::WINDOW_W;
    renderClear(150, 150, 150);
    setColor(0, 0, 0);
    drawText("END GAME STATS", W*0.1, H*0.05, H*0.05);
    drawText("GOLD", W*0.25, H*0.15, H*0.05);
    drawLine(W*0.245, H*0.15, W*0.245, H*(0.2+NUM_PLAYERS*0.05));
    drawText("KILLS", W*0.35, H*0.15, H*0.05);
    drawLine(W*0.345, H*0.15, W*0.345, H*(0.2+NUM_PLAYERS*0.05));
    drawText("DEATHS", W*0.45, H*0.15, H*0.05);
    drawLine(W*0.445, H*0.15, W*0.445, H*(0.2+NUM_PLAYERS*0.05));
    drawText("D DEALT", W*0.55, H*0.15, H*0.05);
    drawLine(W*0.545, H*0.15, W*0.545, H*(0.2+NUM_PLAYERS*0.05));
    drawText("D TAKEN", W*0.65, H*0.15, H*0.05);
    drawLine(W*0.645, H*0.15, W*0.645, H*(0.2+NUM_PLAYERS*0.05));
    drawText("SCORE", W*0.75, H*0.15, H*0.05);
    drawLine(W*0.745, H*0.15, W*0.745, H*(0.2+NUM_PLAYERS*0.05));
    for(int i=0; i<NUM_PLAYERS; i++)
    {
        drawText("PLAYER " + to_str(i+1), W*0.1, H*(0.2+i*0.05), H*0.05);
        drawLine(W*0.1, H*(0.2+i*0.05), W*0.75, H*(0.2+i*0.05));
        drawText(to_str((int)players[i].gold), W*0.25, H*(0.2+i*0.05), H*0.05);
        drawText(to_str(players[i].kills), W*0.35, H*(0.2+i*0.05), H*0.05);
        drawText(to_str(players[i].deaths), W*0.45, H*(0.2+i*0.05), H*0.05);
        drawText(to_str((int)players[i].damageDealt), W*0.55, H*(0.2+i*0.05), H*0.05);
        drawText(to_str((int)players[i].damageTaken), W*0.65, H*(0.2+i*0.05), H*0.05);
        drawText(to_str((int)players[i].getScore()), W*0.75, H*(0.2+i*0.05), H*0.05);
    }
    drawText("PLAYER " + to_str(winner+1) + " WINS", W*0.1, H*0.5, H*0.05);
    if(laggTime <= 0)
        drawText("PRESS SPACEBAR TO RETURN TO THE MAIN MENU", W/10, H*0.9, H*0.05);
    updateScreen();
}
