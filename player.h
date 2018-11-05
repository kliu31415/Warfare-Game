const int NOT_PRESSED = 1e9 + 1111;
texture *t_gold_icon = NULL;
void give_pickup_to_player(player &x, pickup &p);
void fireWeapon(weapon &s);
void updateWeapon(int p, weapon &s);
void subHealth(int p1, int p2, double amt, bool isExplosion = false);
void displayPlayerShopItems(int p);
struct player
{
    vector<weapon> weapons;
    texture *sprite; //each player has a different colored texture
    bool isAI; //haven't implemented AI yet, so always false
    int playerNum; //player number
    SDL_Scancode moveLeft, moveRight, jump; //key bindings
    SDL_Scancode fireUp, fireDown, fireLeft, fireRight;
    SDL_Scancode swapWeapons, pauseGame;
    int pressCnt[256]; //input stuff
    double playerX, playerY; //X and Y position in terms of MAP_W and MAP_H
    double dY; //Y velocity
    int yTime; //Prevents players from sticking to a ceiling by creating a delay before jumping again (because dY=0 when a player reaches the ceiling)
    double gold, maxHP, curHP;
    static constexpr int MAX_JUMPS = 2;
    int jumpsUsed; //maybe people can have multiple jumps?
    int kills, deaths;
    double damageDealt, damageTaken;
    double deathTime; //when a player dies, they take a while to respawn (in milliseconds)
    double fallInjuryF; //determines how much HP a person loses when falling
    double airResistance;
    double jumpVelocity; //variables may affect a specific player's gravity/air resistance
    double gravity;
    double moveSpeed;
    int spawnPoint; //generate a spawn point a little while in advance so people aren't confused
    SDL_Color playerColor; //players have different colors
    //shop modifiers
    double damageMult; //damage multiplier. 1 by default without any items;
    double chestGoldMult; //how much money you get from chests. 1 by default.
    double damageLostRangeFactor; //affects how much damage your projectiles lose with range. 1 by default.
    double projLifespanF;
    double healthRegenRate; //per second
    double fireRateF;
    double damagePerKill;
    double lifeSteal;
    double explosionDamageDealtF, explosionDamageReceivedF, explosionSizeF;
    //end shop modifiers
    set<int> shopItemsBought;
    player(int _playerNum)
    {
        isAI = false;
        sprite = loadTexture("player_sprite.png", 0, 0, 0);
        playerNum = _playerNum;
        SDL_SetTextureAlphaMod(sprite, 180); //make the players a little transparent
        //hard coded stuff for testing the game
        moveLeft = moveRight = jump = fireUp = fireDown = fireLeft = fireRight = swapWeapons = pauseGame = SDL_SCANCODE_NOT_ASSIGNED;
        switch(playerNum)
        {
        case 0:
            moveLeft = SDL_SCANCODE_LEFT;
            moveRight = SDL_SCANCODE_RIGHT;
            jump = SDL_SCANCODE_UP;
            swapWeapons = SDL_SCANCODE_DOWN;
            fireUp = SDL_SCANCODE_KP_8;
            fireDown = SDL_SCANCODE_KP_5;
            fireLeft = SDL_SCANCODE_KP_4;
            fireRight = SDL_SCANCODE_KP_6;
            playerColor = SDL_Color{255, 255, 0};
            break;
        case 1:
            moveLeft = SDL_SCANCODE_H;
            moveRight = SDL_SCANCODE_K;
            jump = SDL_SCANCODE_U;
            swapWeapons = SDL_SCANCODE_J;
            fireUp = SDL_SCANCODE_P;
            fireDown = SDL_SCANCODE_SEMICOLON;
            fireLeft = SDL_SCANCODE_L;
            fireRight = SDL_SCANCODE_APOSTROPHE;
            playerColor = SDL_Color{255, 0, 255};
            break;
        case 2:
            moveLeft = SDL_SCANCODE_S;
            moveRight = SDL_SCANCODE_F;
            jump = SDL_SCANCODE_E;
            swapWeapons = SDL_SCANCODE_D;
            fireUp = SDL_SCANCODE_1;
            fireDown = SDL_SCANCODE_Q;
            fireLeft = SDL_SCANCODE_TAB;
            fireRight = SDL_SCANCODE_W;
            playerColor = SDL_Color{0, 255, 255};
            break;
        case 3:
            playerColor = SDL_Color{255, 64, 64};
            break;
        }
        SDL_SetTextureColorMod(sprite, playerColor.r, playerColor.g, playerColor.b);
    }
    void reset()
    {
        generateSpawnPoint();
        shopItemsBought.clear();
        damageMult = 1;
        chestGoldMult = 1;
        damageLostRangeFactor = 1;
        projLifespanF = 1;
        healthRegenRate = 0;
        fireRateF = 1;
        damagePerKill = 0;
        lifeSteal = 0;
        explosionDamageDealtF = 1;
        explosionDamageReceivedF = 1;
        explosionSizeF = 1;
        //end shop items
        damageDealt = damageTaken = 0;
        weapons.clear();
        weapons.push_back({0, playerNum}); //everyone starts out with the default weapon
        gold = 0;
        jumpVelocity = BASE_JUMP_VELOCITY;
        gravity = BASE_GRAVITY;
        moveSpeed = BASE_MOVE_SPEED;
        maxHP = 1000; //base HP is 1000
        fallInjuryF = BASE_FALLING_INJURY_F;
        airResistance = BASE_AIR_RESISTANCE;
        kills = deaths = damageDealt = damageTaken = 0;
        //for testing purposes give everyone everything
        /*for(int i=0; i<NUM_SHOP_ITEMS; i++)
            addShopItem(i);*/
        //
        respawn();
        fill(pressCnt, end(pressCnt), NOT_PRESSED);
    }
    void addShopItem(int x) //make a modification based on what the player has purchased
    {
        shopItemsBought.insert(x);
        switch(x)
        {
        case 0: //Boots of agility
            fallInjuryF += BASE_FALLING_INJURY_F * 0.1;
            jumpVelocity += BASE_JUMP_VELOCITY * 0.05;
            moveSpeed += BASE_MOVE_SPEED *0.05;
            break;
        case 1: //Mr. J. Szo's Flute
            damageMult += 0.1;
            break;
        case 2: //Suspicious Sack
            chestGoldMult += 0.5;
            break;
        case 3: //JHL Trumpet
            damageLostRangeFactor -= 0.3;
            projLifespanF += 0.2;
            break;
        case 4: //NWS Jacket
            maxHP += 50;
            curHP = maxHP;
            healthRegenRate += 5;
            explosionDamageReceivedF -= 0.1;
            break;
        case 5: //TI 84.5
            fireRateF += 0.1;
            break;
        case 6: //exe knife
            damagePerKill = 0.02;
            break;
        case 7: //sadists machete
            lifeSteal += 0.1;
            break;
        case 8: //math exam
            explosionDamageDealtF += 0.2;
            break;
        case 9: //unidentified flask
            explosionSizeF += 0.1;
            damageDealt += 0.07;
            lifeSteal += 0.05;
            healthRegenRate -= 5;
            break;
        }
    }
    void updateKey(const uint8_t *keystate, SDL_Scancode key)
    {
        //if a key is pressed down, then pressCnt is how many milliseconds it's been pressed down
        //if a key isn't, then pressCnt-NOT_PRESSED is how many milliseconds it hasn't been pressed down
        //immediately when the state is changed, the value is 0
        pressCnt[key] += getFrameLength();
        if(keystate[key])
        {
            if(pressCnt[key] >= NOT_PRESSED)
                pressCnt[key] = 0;
        }
        else if(pressCnt[key] < NOT_PRESSED)
            pressCnt[key] = NOT_PRESSED;
    }
    bool intersects() //add makes the vertical collision box a bit bigger so players don't get stuck on the ground when moving horizontally
    {
        if(intersectsBlocks(playerX, playerY))
            return true;
        for(int j=0; j<NUM_PLAYERS; j++) //make sure this person doesn't intersect with other players (unit collision is bad)
        {
            player &i = players[j];
            if(i.playerNum==playerNum || i.deathTime!=-1) //obviously it's going to intersect with itself - just skip this case
                continue; //also ignore the case when somebody is dead because they don't occupy space if they're dead
            if(rectsIntersect({(playerX+0.15)*BLOCK_SIZE, (playerY+0.15)*BLOCK_SIZE, BLOCK_SIZE*0.7, BLOCK_SIZE*0.7},
                              {(i.playerX+0.15)*BLOCK_SIZE, (i.playerY+0.15)*BLOCK_SIZE, BLOCK_SIZE*0.7, BLOCK_SIZE*0.7}))
               return true;
        }
        return false;
    }
    void moveAlive()
    {
        int INTERVALS = getFrameLength() * INTERVAL_MOD, INTV2 = 10; //move the player every 1 millisecond (frameLength is the length of this frame in milliseconds)
        if(pressCnt[moveLeft]<NOT_PRESSED || pressCnt[moveRight]<NOT_PRESSED)
        {
            double curSpeed = moveSpeed * getFrameLength();
            int mult = 1;
            if(pressCnt[moveLeft] < pressCnt[moveRight])
            mult = -1;
            for(int i=0; i<INTERVALS; i++) //move the player in very small intervals to their next state
            {
                playerX += mult*curSpeed / INTERVALS;
                if(intersects()) //once they intersect with something, stop moving the player
                {
                    for(int j=0; j<INTV2 && intersects(); j++) //correct a little in the opposite direction
                        playerX -= mult*curSpeed / (INTV2 * INTERVALS);
                    break;
                }
            }
        }
        if(pressCnt[jump] < NOT_PRESSED)
        {
            if(dY>=0 && jumpsUsed != MAX_JUMPS) //reminder: yTime prevents people from sticking to the ceiling
            {
                dY = jumpVelocity;
                jumpsUsed++;
            }
        }
        if(dY == 0)
            yTime++;
        else yTime = 0;
        if(yTime >= 5)
            jumpsUsed = 0;
        dY += gravity * getFrameLength(); //natural force of gravity (in case the cell below the player is empty)
        dY *= pow(1 - airResistance, getFrameLength());
        double deltaY = dY * getFrameLength() / INTERVALS;
        if(dY > 0) //falling
        {
            for(int i=0; i<INTERVALS; i++)
            {
                playerY += deltaY;
                if(intersects())
                {
                    for(int j=0; j<INTV2 && intersects(); j++)
                        playerY -= deltaY / INTV2;
                    if(dY > fallInjuryF) //is this player falling fast enough to lose HP?
                        subHealth(playerNum, playerNum, pow(150*(dY - fallInjuryF), 2) / fallInjuryF);
                    dY = 0;
                    break;
                }
            }
        }
        else if(dY < 0) //rising
        {
            for(int i=0; i<INTERVALS; i++)
            {
                playerY += deltaY;
                if(intersects())
                {
                    for(int j=0; j<INTV2 && intersects(); j++)
                        playerY -= deltaY / INTV2;
                    dY = 0;
                    break;
                }
            }
        }
    }
    void attackAlive()
    {
        //note: weapons[0] is always the one currently in use
        if(weapons.size() == 0) //does the player have any weapons?
            return;
        if(weapons.size()==2 && pressCnt[swapWeapons]==0)
        {
            swap(weapons[0], weapons[1]);
            weapons[0].rotation = weapons[1].rotation;
            weapons[0].delay = weapon_base[weapons[0].type].fireRate * 1/fireRateF; //otherwise you could fire both weapons at the same time
        }
        int a = pressCnt[fireUp], b = pressCnt[fireRight], c = pressCnt[fireDown], d = pressCnt[fireLeft];
        if((a<NOT_PRESSED || b<NOT_PRESSED || c<NOT_PRESSED || d<NOT_PRESSED) && min(min(a, b), min(c, d))>FIRE_DELAY)
        {//remember that NOT_PRESSED is a very large number
            if((a>=NOT_PRESSED && a-NOT_PRESSED<FIRE_DELAY) ||
               (b>=NOT_PRESSED && b-NOT_PRESSED<FIRE_DELAY) ||
               (c>=NOT_PRESSED && c-NOT_PRESSED<FIRE_DELAY) ||
               (d>=NOT_PRESSED && d-NOT_PRESSED<FIRE_DELAY)) //FIRE_DELAY also applies to releasing keys
                return;
            int pressedV = min(NOT_PRESSED-1, min(a, c)); //NOT_PRESSED-1 is a ridiculously large number so this should work
            int pressedH = min(NOT_PRESSED-1, min(b, d));
            if(a == pressedV) //FIRE UP
            {
                if(b == pressedH)
                    weapons[0].setRotation(1);
                else if(d == pressedH)
                    weapons[0].setRotation(7);
                else weapons[0].setRotation(0);
            }
            else if(c == pressedV) //FIRE DOWN
            {
                if(b == pressedH)
                    weapons[0].setRotation(3);
                else if(d == pressedH)
                    weapons[0].setRotation(5);
                else weapons[0].setRotation(4);
            }
            else
            {
                if(b == pressedH)
                    weapons[0].setRotation(2);
                else weapons[0].setRotation(6);
            }
            fireWeapon(weapons[0]);
            if(weapons[0].ammo == 0)
            {
                weapons.erase(weapons.begin());
                for(auto &i: weapons) //this is affected by fire rate too I guess
                    i.delay = weapon_base[i.type].fireRate * fireRateF;
            }
        }
    }
    void processAlive()
    {
        moveAlive(); //movement commands
        attackAlive(); //weapon commands
        curHP = min(curHP + healthRegenRate*getFrameLength()/1000, maxHP); //regenerate health
        //check if they got a pickup
        for(int i=0; i<pickups.size(); i++)
        {
            pickup &x = pickups[i];
            if(rectsIntersect({playerX*BLOCK_SIZE, playerY*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE},
                              {x.x*BLOCK_SIZE, x.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE})
                            && x.waitTime==0)
            {
                //do stuff
                give_pickup_to_player(*this, pickups[i]);
                //delete this pickup
                pickups.erase(pickups.begin() + i);
                i--;
            }
        }
    }
    void processDead()
    {
        if(spawnPoint == -1)
        {
            generateSpawnPoint();
        }
    }
    void process(const uint8_t *keystate)
    {
        //process input first
        updateKey(keystate, moveRight);
        updateKey(keystate, moveLeft);
        updateKey(keystate, jump);
        updateKey(keystate, fireUp);
        updateKey(keystate, fireDown);
        updateKey(keystate, fireLeft);
        updateKey(keystate, fireRight);
        updateKey(keystate, swapWeapons);
        updateKey(keystate, pauseGame);
        for(auto &i: weapons)
            updateWeapon(playerNum, i);
        if(deathTime == -1)
            processAlive();
        else processDead();
    }
    int hOffset, pxWidth, vOffset, baseFontSize;
    void renderAlive()
    {
        using namespace settings;
        renderCopy(sprite, playerX * BLOCK_SIZE, playerY * BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
        drawText(to_str((int)(ceil(curHP))) + "/" + to_str((int)ceil(maxHP)), hOffset + pxWidth/10, vOffset + WINDOW_H/36, baseFontSize*2);
    }
    void renderDead()
    {
        int t = RESPAWN_TIME - (getTicks() - deathTime);
        if(t < 3000) //show where they're spawning a little in advance
        {
            pii z = gameMaps[currentMap].playerSpawnPoints[spawnPoint];
            fillCircle((z.A+0.5)*BLOCK_SIZE, (z.B+0.5)*BLOCK_SIZE, BLOCK_SIZE, playerColor.r, playerColor.g, playerColor.b, 255 - t*255/4000);
        }
        int timeLeft = (t + 999) / 1000;
        drawText(to_str(timeLeft) + "s until respawn", hOffset + pxWidth/10, vOffset + settings::WINDOW_H/36, baseFontSize*1.5);
        if(timeLeft <= 0) //check if it's time for them to respawn
        {
            respawn();
        }
    }
    void render()
    {
        using namespace settings;
        hOffset = MAP_W*BLOCK_SIZE; //horizontal offset in pixels of the sidebar
        pxWidth = WINDOW_W - hOffset; //width of the sidebar in pixels
        vOffset = playerNum * WINDOW_H/4; //vertical offset in pixels of this player's sidebar
        baseFontSize = WINDOW_H/40; //standard font size
        fillRect(hOffset + pxWidth/10, vOffset + WINDOW_H/36, pxWidth*4/5, baseFontSize*2, 255, 0, 0); //display health bar
        fillRect(hOffset + pxWidth/10, vOffset + WINDOW_H/36, max(0, (int)(pxWidth*4/5*curHP/maxHP)), baseFontSize*2, 0, 255, 0);
        if(weapons.size()) //does this player have any weapons?
        {
            if(deathTime == -1) //player is alive
                weapons[0].render(playerX, playerY);
            renderCopy(weapon_base[weapons[0].type].t, hOffset, vOffset + WINDOW_H/36+baseFontSize*2, baseFontSize*2, baseFontSize*2);
            drawText(to_str(weapons[0].ammo), hOffset + baseFontSize*2, vOffset + WINDOW_H/36+baseFontSize*2, baseFontSize*2);
            if(weapons[0].delay > 0) //(eye candy) circle to show when a weapon is ready to fire again
            {//fills a sector of a circle
                double val = 2*PI * weapons[0].delay / (double)weapon_base[weapons[0].type].fireRate;
                setColor(0, 0, 0, 100);
                int r = baseFontSize, x = hOffset + baseFontSize, y = vOffset + WINDOW_H/36 + baseFontSize*3;
                int r2 = r * r;
                for(int X=x-r; X<=x+r; X++)
                {
                    for(int Y=y-r; Y<=y+r; Y++)
                    {
                        if((X-x)*(X-x) + (Y-y)*(Y-y) <= r2 && fmod(-atan2(Y-y, X-x)+3*PI/2, 2*PI) < val)
                            drawPoint(X, Y);
                    }
                }
            }
            if(weapons.size() == 2) //show second weapon in sidebar
            {
                renderCopy(weapon_base[weapons[1].type].t, hOffset, vOffset + WINDOW_H/36+baseFontSize*4, baseFontSize*2, baseFontSize*2);
                drawText(to_str(weapons[1].ammo), hOffset + baseFontSize*2, vOffset + WINDOW_H/36+baseFontSize*4, baseFontSize*2);
            }
        }
        //show items
        drawText("K:" + to_str(kills), hOffset, vOffset + WINDOW_H/36 + baseFontSize*6, baseFontSize*2);
        drawText("D:" + to_str(deaths), hOffset + baseFontSize*5, vOffset + WINDOW_H/36 + baseFontSize*6, baseFontSize*2);
        //
        fillRect(hOffset, vOffset - WINDOW_H/100, pxWidth, WINDOW_H/100, 50, 50, 50);
        renderCopy(t_gold_icon, hOffset + pxWidth/2, vOffset + WINDOW_H/36 + baseFontSize*2, baseFontSize*2, baseFontSize*2);
        drawText(to_str((int)gold), hOffset + pxWidth/2 + baseFontSize*2, vOffset + WINDOW_H/36 + baseFontSize*2, baseFontSize*2);
        renderCopy(sprite, hOffset + 0.00833*pxWidth, vOffset + WINDOW_H/36 + 0.00833*pxWidth, pxWidth/12, pxWidth/12);
        displayPlayerShopItems(playerNum);
        if(deathTime == -1) //player is alive
            renderAlive();
        else renderDead();
    }
    double getScore()
    {
        return gold + damageDealt/20 + kills*50;
    }
    void generateSpawnPoint()
    {
        while(1)
        {
            spawnPoint = randuz() % gameMaps[currentMap].playerSpawnPoints.size();
            playerX = gameMaps[currentMap].playerSpawnPoints[spawnPoint].A;
            playerY = gameMaps[currentMap].playerSpawnPoints[spawnPoint].B;
            if(!intersects()) //make sure nobody spawns on top of something else
                break;
        }
    }
    void respawn()
    {
        dY = 0;
        yTime = 0;
        jumpsUsed = 0;
        deathTime = -1;
        curHP = maxHP;
    }
};
