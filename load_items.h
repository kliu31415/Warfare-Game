void loadItems()
{
    t_dirt1 = loadTexture("dirt1.png");
    t_rock1 = loadTexture("rock1.png");
    t_gold_icon = loadTexture("gold_icon.png", 255, 255, 255);
    t_explosion1 = loadTexture("explosion1_v2.png", 0, 0, 0);
    for(int i=0; i<PICKUP_TYPES; i++)
        t_pickup[i] = loadTexture("pickup.png", 0, 0, 0);
    SDL_SetTextureColorMod(t_pickup[0], 200, 150, 200);
    SDL_SetTextureColorMod(t_pickup[1], 100, 200, 150);
    SDL_SetTextureColorMod(t_pickup[2], 150, 50, 200);
    SDL_SetTextureColorMod(t_pickup[3], 100, 150, 200);
}
