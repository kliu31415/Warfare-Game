//PROJECTILES
bool projIntersectsBlocks(double hitboxOffset, double x, double y)
{
    for(int i=max(int(x-1), 0); i<=min(int(x+1), MAP_W-1); i++)
    {
        for(int j=max(int(y-1), 0); j<=min(int(y+1), MAP_H-1); j++)
        {
            if(gameMaps[currentMap].blocks[i][j] == gameMap::block::empty) //if this cell is empty we don't care about collisions
                continue;
            if(rectsIntersect({x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE*0.8, BLOCK_SIZE*0.8},
                            {(i+hitboxOffset)*BLOCK_SIZE, (j+hitboxOffset)*BLOCK_SIZE, BLOCK_SIZE*(1-hitboxOffset*2), BLOCK_SIZE*(1-hitboxOffset*2)}))
                                //checks if the player intersects with the block
                {
                    return true;
            }
        }
    }
    return false;
}
int projIntersectsPlayer(double hitboxOffset, int pNum, double x, double y)
{
    for(int j=0; j<NUM_PLAYERS; j++)
    {
        player &i = players[j];
        //cnt != pNum ensures that your own projectiles can't harm you
        //also ignore dead players
        if(j!=pNum && i.deathTime==-1 && rectsIntersect({i.playerX*BLOCK_SIZE, i.playerY*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE},
                            {(x+hitboxOffset)*BLOCK_SIZE, (y+hitboxOffset)*BLOCK_SIZE, BLOCK_SIZE*(1-hitboxOffset*2), BLOCK_SIZE*(1-hitboxOffset*2)}))
                            return j;
    }
    return -1;
}
void processProjectiles()
{
    const int INTERVALS = getFrameLength() * INTERVAL_MOD; //we're checking moving the projectile once per millisecond
    for(int i=0; i<projectiles.size(); i++)
    {
        projectile &P = projectiles[i];
        P.lifespan -= getFrameLength();
        P.damage *= pow(P.decayRate, getFrameLength());
        if(P.lifespan <= 0) //projectile is dead now
        {
            if(P.explosionRadius != -1) //exploding projectiles just explode when they run out of time
                explosions.push_back(explosion(P.playerNum, P.x+0.5, P.y+0.5, P.explosionRadius, P.damage, P.explosionDuration));
            projectiles.erase(projectiles.begin() + i);
            i--;
            continue;
        }
        P.dY += P.gravity * getFrameLength();
        double dX = P.dX / INTERVAL_MOD;
        double dY = P.dY / INTERVAL_MOD;
        bool collision = false;
        for(int j=0; j<INTERVALS; j++)
        {
            P.x += dX;
            P.y += dY;
            int z = projIntersectsPlayer(projectile_base[P.type].hitbox, P.playerNum, P.x, P.y);
            if(z != -1)
            {
                if(P.explosionRadius != -1)
                    explosions.push_back(explosion(P.playerNum, P.x+0.5, P.y+0.5, P.explosionRadius, P.damage, P.explosionDuration));
                else subHealth(P.playerNum, z, P.damage);
                projectiles.erase(projectiles.begin() + i);
                i--;
                collision = true;
                break;
            }
            if(!P.spectral && projIntersectsBlocks(projectile_base[P.type].hitbox, P.x, P.y))
            {
                if(P.explosionRadius != -1)
                    explosions.push_back(explosion(P.playerNum, P.x+0.5, P.y+0.5, P.explosionRadius, P.damage, P.explosionDuration));
                projectiles.erase(projectiles.begin() + i);
                i--;
                collision = true;
                break;
            }
        }
        if(P.x<0 || P.x>MAP_W || P.y<0 || P.y>MAP_H) //it's out of the map (only should affects spectral projectiles)
        {
            projectiles.erase(projectiles.begin() + i);
            i--;
            collision = true;
            continue;
        }
    }
}
void give_pickup_to_player(player &x, pickup &p)
{
    double z = randf() - 0.0001;
    weapon w(z*NUM_WEAPONS, x.playerNum); //chests with weapons give a random weapon W.E.P. (with equal probability)
    string wname = weapon_base[w.type].name;
    pickup_spawn_occupied[p.spawnPoint] = false;
    int gold = 10 + randf() * 49.999; //chests with gold give a random number from 10 - 60 W.E.P.
    int baseFontSize = settings::WINDOW_H/40;
    switch(p.type)
    {
    case 0: //cash
        x.gold += gold * x.chestGoldMult; //a shop item increases gold earned
        disapText.push_back({to_str(gold) + " gold", p.x*BLOCK_SIZE - baseFontSize*1.5, p.y*BLOCK_SIZE, baseFontSize, 3000, -255/3.0, 255, 255, 0});
        break;
    case 1: //weapon
        disapText.push_back({wname, p.x*BLOCK_SIZE - wname.size()/4*baseFontSize, p.y*BLOCK_SIZE, baseFontSize, 3000, -255/3.0, 255, 255, 0});
        if(x.weapons.size() == 2)
        {
            if(x.weapons[0].type == w.type) //just give the ammo if this person already has the weapon
                x.weapons[0].ammo += weapon_base[x.weapons[0].type].ammo;
            else if(x.weapons[1].type == w.type) //just give the ammo if this person already has the weapon
                x.weapons[1].ammo += weapon_base[x.weapons[1].type].ammo;
            else x.weapons[1] = w; //the weapon that is NOT equipped is replaced by the new one
        }
        else
        {
            if(x.weapons[0].type == w.type) //just give the ammo if this person already has the weapon
                x.weapons[0].ammo += weapon_base[x.weapons[0].type].ammo;
            else x.weapons.push_back(w);
        }
        break;
    default:
        printf("%s%d%s%d%s%d%s", "Error: ", p.type, " is an invalid pickup type (must be a number from 0 to ", PICKUP_TYPES, " (line ", __LINE__, ")\n");
    }
}
void fireWeapon(weapon &s)
{
    player &p = players[s.playerNum];
    double ox = p.playerX, oy = p.playerY; //original x and y coordinates
    while(s.ammo>0 && s.delay<=getFrameLength())
    //check if weapon is ready to fire (the >frameLength ensures smoother firing for stuff that isn't divisible by the fps (60))
    {
        _weapon_base w = weapon_base[s.type];
        w.damage *= p.damageMult;
        w.decayRate = 1 - (1-w.decayRate) * p.damageLostRangeFactor;
        w.projLifespan *= p.projLifespanF;
        s.ammo--;
        s.delay += w.fireRate * 1/p.fireRateF;
        double angle1 = PI/2 - s.rotation*PI/4; //original angle
        for(int i=0; i<w.bulletCount; i++)
        {
            double x = ox, y = oy;
            double angle2 = angle1;
            if(randuz() & 1)
                angle2 += randf() * w.deviation;
            else angle2 -= randf() * w.deviation;
            double dX = cos(angle2) * w.velocity;
            double dY = -sin(angle2) * w.velocity; //remember that the origin is in the top left, not the bottom left
            //The projectile should ideally not start at the center of the player - there should be a little offset
            int INTERVALS = 100 * w.velocity * INTERVAL_MOD; //make sure we don't miss a point blank shot (the offset may skip an object if the bullet is too fast)
            INTERVALS = max(INTERVALS, 10); //need at least 10 to make it smoothish (slow bullets might have INTERVALS = 0 or 1 which is bad)
            bool collision = false;
            //smoother firing for things that fire bullets multiple times in one frame (used for the second part of the equations)
            double diffX = (dX/w.velocity)/INTERVALS * (0.8 + max(0.0, w.velocity *(s.delay-w.fireRate)));
            double diffY = (dY/w.velocity)/INTERVALS * (0.8 + max(0.0, w.velocity *(s.delay-w.fireRate)));
            for(int j=0; j<INTERVALS; j++)
            {
                x += diffX;
                y += diffY;
                int z = projIntersectsPlayer(projectile_base[w.projType].hitbox, s.playerNum, x, y);
                if(z != -1)
                {
                    if(w.explosionRadius != -1) //if it explodes the player will be damaged later anyway
                        explosions.push_back(explosion(s.playerNum, x+0.5, y+0.5, w.explosionRadius, w.damage, w.explosionDuration));
                    else subHealth(s.playerNum, z, w.damage);
                    collision = true;
                    break;
                }
                if(!w.spectral && projIntersectsBlocks(projectile_base[w.projType].hitbox, x, y))
                {
                    if(w.explosionRadius != -1)
                        explosions.push_back(explosion(s.playerNum, x+0.5, y+0.5, w.explosionRadius, w.damage, w.explosionDuration));
                    collision = true;
                    break;
                }
            }
            if(!collision)
                projectiles.push_back({w.projType, w.spectral, w.projGravityF*BASE_GRAVITY, s.playerNum, w.projLifespan, w.decayRate,
                                       w.damage, x, y, dX, dY, w.explosionRadius, w.explosionDuration});
        }
    }
}
void updateWeapon(int p, weapon &s)
{
    s.delay -= getFrameLength();
    s.delay = max(s.delay, 0.0);
}
void subHealth(int _p1, int _p2, double amt, bool isExplosion/*=false*/) //p1 deals damage to p2
{
    player &p1 = players[_p1];
    player &p2 = players[_p2];
    if(isExplosion)
    {
        amt *= p1.explosionDamageDealtF;
        amt *= p2.explosionDamageReceivedF;
    }
    p2.damageTaken += min(amt, p2.curHP);
    p1.damageDealt += min(amt, p2.curHP);
    if(p1.deathTime == -1) //dead people can't lifesteal
        p1.curHP = min(p1.maxHP, p1.curHP + min(p2.curHP, amt)*p1.lifeSteal);
    p2.curHP -= amt;
    if(p2.curHP <= 0) //check if they're dead
    {
        int baseFontSize = settings::WINDOW_H/40;
        disapText.push_back({"DEAD", p2.playerX*BLOCK_SIZE - baseFontSize, p2.playerY*BLOCK_SIZE, baseFontSize, 3000, -255/3.0, 255, 0, 0});
        p1.kills++;
        p2.deaths++;
        p2.deathTime = getTicks();
        double goldStolen = p2.gold * BASE_GOLD_STOLEN;
        disapText.push_back({to_str((int)goldStolen) + " gold", p1.playerX*BLOCK_SIZE - baseFontSize*1.5,
                             p1.playerY*BLOCK_SIZE, baseFontSize, 3000, -255/3.0, 255, 120, 0});
        p1.gold += goldStolen;
        p2.gold -= goldStolen;
        p1.damageMult += p1.damagePerKill;
        p2.spawnPoint = -1;
    }
}
void handleExplosions()
{
    for(int i=0; i<explosions.size(); i++)
    {
        explosion &e = explosions[i];
        //the explosion first quickly reaches BASE_ALPHA then slowly goes away
        uint8_t a;
        int ALPHA_INC_TIME = explosion::ALPHA_INCREASE_TIME_F * e.duration;
        if(e.timeLeft >= ALPHA_INC_TIME)
        {
            a = explosion::BASE_ALPHA * (1 - //increases nonlinearly up to the peak
            pow((e.timeLeft - ALPHA_INC_TIME) / (double)(e.duration - ALPHA_INC_TIME), 2.5));
        }
        else a = explosion::BASE_ALPHA * e.timeLeft/ALPHA_INC_TIME;
        SDL_SetTextureAlphaMod(t_explosion1, a);
        renderCopy(t_explosion1, BLOCK_SIZE*(e.x-e.size), BLOCK_SIZE*(e.y-e.size), BLOCK_SIZE*e.size*2, BLOCK_SIZE*e.size*2);
        e.timeLeft -= getFrameLength();
        if(e.timeLeft <= 0)
        {
            explosions.erase(explosions.begin() + i);
            i--;
            continue;
        }
    }
}
void explosion::damage_players()
{
    for(int i=0; i<NUM_PLAYERS; i++)
    {
        player &p = players[i];
        if((!friendlyFire && p.playerNum==playerNum) || p.deathTime!=-1)
            continue;
        double d = sqrt(pow((p.playerX+0.5)-x, 2) + pow((p.playerY+0.5)-y, 2)); //(X, Y) of an explosion is its center, not top left corner
        if(d < size+0.5) //player radius is approximately 0.5
            subHealth(playerNum, p.playerNum, damage * pow((size+0.5-d)/(size+0.5), 1.5), true); //pow() makes it not linear
    }
}
explosion::explosion(int pnum, double _x, double _y, double _size, double _damage, int dur)
{
    playerNum = pnum;
    x = _x;
    y = _y;
    size = _size * players[pnum].explosionSizeF;
    damage = _damage;
    if(dur < 0)
    {
        dur = -dur;
        friendlyFire = false;
    }
    else friendlyFire = true;
    timeLeft = duration = dur;
    damage_players();
}
