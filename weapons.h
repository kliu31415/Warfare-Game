const int NUM_PROJECTILES = 9;
const int NUM_WEAPONS = 23;
struct _projectile_base
{
    texture *t;
    double hitbox; //Offset from edges. Ex. 0.4 means the projectile occupies the middle 0.2*0.2.
};
_projectile_base projectile_base[NUM_PROJECTILES];
struct _weapon_base
{
    texture *t;
    string name;
    int projType;
    double velocity; //blocks per millisecond
    int ammo;
    double fireRate; //milliseconds time in between bullets (ex. 500 means 2 shots/second)
    int projLifespan; //how many milliseconds will the projectile last?
    double decayRate; //damage multiplier per millisecond travelled
    double damage; //damage at point blank
    double deviation; //weapons usually don't shoot at a perfect angle. deviation = max degrees up or down.
    int bulletCount; //number of bullets fired per shot (Ex. shotgun may have 5)
    double projGravityF; //how much does gravity affect this projectile?
    bool spectral = false; //does it pass through walls? (false by default)
    double explosionRadius = -1; //-1 means it doesn't explode
    double explosionDuration = 2000; //a negative value means that this explosion has no friendly fire (doesn't damage the player creating it)
};
_weapon_base weapon_base[NUM_WEAPONS];
struct projectile
{
    double x, y;
    double dX, dY; //blocks per second
    int type;
    double decayRate; //damage multiplier per second travelled
    double damage;
    int lifespan; //how many milliseconds will this projectile last?
    int playerNum; //which player shot this projectile?
    double gravity;
    bool spectral;
    double explosionRadius;
    double explosionDuration;
    projectile(int t, bool s, double g, int pNum, double life, double dR, double dmg, double _x, double _y, double dx, double dy, double eR, double eD)
    {
        spectral = s;
        damage = dmg;
        gravity = g;
        type = t;
        playerNum = pNum;
        lifespan = life;
        decayRate = dR;
        x = _x;
        y = _y;
        dX = dx;
        dY = dy;
        explosionRadius = eR;
        explosionDuration = eD;
    }
    void render()
    {
        renderCopyEx(projectile_base[type].t, x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, TO_DEG(atan2(dY, dX)));
    }
};
vector<projectile> projectiles;
void damagePlayer(int z, int damage);
int projIntersectsPlayer(double hitboxOffset, int pNum, double x, double y);
bool projIntersectsBlocks(double hitboxOffset, double x, double y);
struct weapon
{
    constexpr static double OFFSET = 0.5; //graphical offset from center when firing in a certain direction
    int type;
    int playerNum; //who owns this weapon?
    int rotation; //0 = up and it goes clockwise to 7
    int ammo;
    double delay; //if a shot has been fired recently, then wait a bit
    weapon(int t, int p)
    {
        type = t;
        playerNum = p;
        ammo = weapon_base[t].ammo;
        rotation = 2;
        delay = 0;
    }
    void setRotation(int r)
    {
        rotation = r;
    }
    void render(double x, double y)
    {
        switch(rotation)
        {
        case 0:
            renderCopyEx(weapon_base[type].t, (x-OFFSET)*BLOCK_SIZE, (y-OFFSET*2)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 270);
            break;
        case 1:
            renderCopyEx(weapon_base[type].t, (x)*BLOCK_SIZE, (y-OFFSET*2)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 315);
            break;
        case 2:
            renderCopyEx(weapon_base[type].t, x*BLOCK_SIZE, (y-OFFSET)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 0);
            break;
        case 3:
            renderCopyEx(weapon_base[type].t, (x)*BLOCK_SIZE, (y)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 45);
            break;
        case 4:
            renderCopyEx(weapon_base[type].t, (x-OFFSET)*BLOCK_SIZE, (y)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 90);
            break;
        case 5:
            renderCopyEx(weapon_base[type].t, (x-OFFSET*2)*BLOCK_SIZE, (y)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 315, SDL_FLIP_HORIZONTAL);
            break;
        case 6:
            renderCopyEx(weapon_base[type].t, (x-OFFSET*2)*BLOCK_SIZE, (y-OFFSET)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 0, SDL_FLIP_HORIZONTAL);
            break;
        case 7:
            renderCopyEx(weapon_base[type].t, (x-OFFSET*2)*BLOCK_SIZE, (y-OFFSET*2)*BLOCK_SIZE, BLOCK_SIZE*2, BLOCK_SIZE*2, 45, SDL_FLIP_HORIZONTAL);
            break;
        }
    }
};
vector<weapon> weapons;
texture *t_explosion1 = NULL;
struct explosion
{
    constexpr static double ALPHA_INCREASE_TIME_F = 0.8; //when should we start decreasing alpha? (1 - this var)
    constexpr static uint8_t BASE_ALPHA = 255; //peak alpha value
    double x, y, size, damage;
    int duration;
    int playerNum, timeLeft;
    bool friendlyFire;
    explosion(int pnum, double _x, double _y, double _size, double _damage, int dur);
    void damage_players();
};
vector<explosion> explosions;
void initWeapons()
{
    //NOTE: weapon 0 is the default weapon
    projectile_base[0].t = loadTexture("P_foam.png", 255, 255, 255);
    projectile_base[0].hitbox = 0.35;
    weapon_base[0].t = loadTexture("W_foamblaster.png", 255, 255, 255);
    weapon_base[0].name = "Foam Blaster";
    weapon_base[0].projType = 0; //foam
    weapon_base[0].ammo = 20;
    weapon_base[0].fireRate = 500;
    weapon_base[0].velocity = 0.025;
    weapon_base[0].projLifespan = 2000;
    weapon_base[0].damage = 220;
    weapon_base[0].decayRate = 0.99965; //0.7046 per second
    weapon_base[0].deviation = TO_RAD(5);
    weapon_base[0].bulletCount = 1;
    weapon_base[0].projGravityF = 0.03;
    //weapon 1
    projectile_base[1].t = loadTexture("P_whitebullet.png", 0, 0, 0);
    projectile_base[1].hitbox = 0.35;
    weapon_base[1].t = loadTexture("W_Jshotgun.png", 255, 255, 255);
    weapon_base[1].name = "J Shotgun";
    weapon_base[1].projType = 1; //white bullet
    weapon_base[1].ammo = 12;
    weapon_base[1].fireRate = 600;
    weapon_base[1].velocity = 0.03;
    weapon_base[1].projLifespan = 1000;
    weapon_base[1].damage = 150;
    weapon_base[1].decayRate = 0.9994; //0.5487 per second
    weapon_base[1].deviation = TO_RAD(8);
    weapon_base[1].bulletCount = 8;
    weapon_base[1].projGravityF = 0.05;
    //weapon 2
    weapon_base[2].t = loadTexture("W_Kgun.png", 255, 255, 255);
    weapon_base[2].name = "K Gun";
    weapon_base[2].projType = 1; //white bullet
    weapon_base[2].ammo = 100;
    weapon_base[2].fireRate = 50;
    weapon_base[2].velocity = 0.035;
    weapon_base[2].projLifespan = 2000;
    weapon_base[2].damage = 90;
    weapon_base[2].decayRate = 0.9998; //0.8187/s
    weapon_base[2].deviation = TO_RAD(12);
    weapon_base[2].bulletCount = 1;
    weapon_base[2].projGravityF = 0.3;
    //weapon 3
    weapon_base[3].t = loadTexture("W_Deathball.png", 255, 255, 255);
    weapon_base[3].name = "Death Ball";
    weapon_base[3].projType = 1; //white bullet
    weapon_base[3].ammo = 1000;
    weapon_base[3].fireRate = 3;
    weapon_base[3].velocity = 0.035;
    weapon_base[3].projLifespan = 100;
    weapon_base[3].damage = 10;
    weapon_base[3].decayRate = 0.9994; //0.5487/s
    weapon_base[3].deviation = TO_RAD(180);
    weapon_base[3].bulletCount = 1;
    weapon_base[3].projGravityF = 0;
    //weapon 4
    projectile_base[2].t = loadTexture("P_redlaser.png", 0, 0, 0);
    projectile_base[2].hitbox = 0.15;
    weapon_base[4].t = loadTexture("W_KL600laser.png", 255, 255, 255);
    weapon_base[4].name = "KL600 Laser";
    weapon_base[4].projType = 2; //red laser
    weapon_base[4].ammo = 250;
    weapon_base[4].fireRate = 15;
    weapon_base[4].velocity = 0.04;
    weapon_base[4].projLifespan = 2000;
    weapon_base[4].damage = 60;
    weapon_base[4].decayRate = 0.9999; //0.9048/s
    weapon_base[4].deviation = TO_RAD(3);
    weapon_base[4].bulletCount = 1;
    weapon_base[4].projGravityF = 0;
    //weapon 5
    weapon_base[5].t = loadTexture("W_Foobarcannon.png", 255, 255, 255);
    weapon_base[5].name = "Foobar Cannon";
    weapon_base[5].projType = 1; //white bullet
    weapon_base[5].ammo = 12;
    weapon_base[5].fireRate = 400;
    weapon_base[5].velocity = 0.03;
    weapon_base[5].projLifespan = 2000;
    weapon_base[5].damage = 40;
    weapon_base[5].decayRate = 0.9995; //0.6065/s
    weapon_base[5].deviation = TO_RAD(15);
    weapon_base[5].bulletCount = 24;
    weapon_base[5].projGravityF = 0.2;
    //weapon 6
    weapon_base[6].t = loadTexture("W_QuxQuxCannon.png", 255, 255, 255);
    weapon_base[6].name = "QuxQux Cannon";
    weapon_base[6].projType = 2; //red laser
    weapon_base[6].ammo = 9;
    weapon_base[6].fireRate = 300;
    weapon_base[6].velocity = 0.04;
    weapon_base[6].projLifespan = 2000;
    weapon_base[6].damage = 50;
    weapon_base[6].decayRate = 0.9998; //0.8187/s
    weapon_base[6].deviation = TO_RAD(10);
    weapon_base[6].bulletCount = 50;
    weapon_base[6].projGravityF = 0;
    //weapon 7
    projectile_base[3].t = loadTexture("P_Zap.png", 255, 255, 255);
    projectile_base[3].hitbox = 0.4;
    weapon_base[7].t = loadTexture("W_Zappy.png", 255, 255, 255);
    weapon_base[7].name = "Zappy";
    weapon_base[7].projType = 3; //Zap
    weapon_base[7].ammo = 1000;
    weapon_base[7].fireRate = 3;
    weapon_base[7].velocity = 0.2;
    weapon_base[7].projLifespan = 2000;
    weapon_base[7].damage = 7;
    weapon_base[7].decayRate = 1; //1.0000/s
    weapon_base[7].deviation = TO_RAD(0);
    weapon_base[7].bulletCount = 1;
    weapon_base[7].projGravityF = 0;
    //weapon 8
    weapon_base[8].t = loadTexture("W_GuanoBall.png", 255, 255, 255);
    weapon_base[8].name = "Guano Ball";
    weapon_base[8].projType = 1; //white bullet
    weapon_base[8].ammo = 500;
    weapon_base[8].fireRate = 15;
    weapon_base[8].velocity = 0.003;
    weapon_base[8].projLifespan = 5000;
    weapon_base[8].damage = 30;
    weapon_base[8].decayRate = 0.9999; //0.9048/s
    weapon_base[8].deviation = TO_RAD(180);
    weapon_base[8].bulletCount = 1;
    weapon_base[8].projGravityF = 0.7;
    //weapon 9
    weapon_base[9].t = loadTexture("W_NewtonsGun.png", 255, 255, 255);
    weapon_base[9].name = "Newton's Gun";
    weapon_base[9].projType = 1; //white bullet
    weapon_base[9].ammo = 30;
    weapon_base[9].fireRate = 150;
    weapon_base[9].velocity = 0.04;
    weapon_base[9].projLifespan = 5000;
    weapon_base[9].damage = 80;
    weapon_base[9].decayRate = 0.9997; //0.7408/s
    weapon_base[9].deviation = TO_RAD(5);
    weapon_base[9].bulletCount = 5;
    weapon_base[9].projGravityF = -0.25;
    //weapon 10
    weapon_base[10].t = loadTexture("W_LaserBubbler.png", 255, 255, 255);
    weapon_base[10].name = "Laser Bubbler";
    weapon_base[10].projType = 2; //red laser
    weapon_base[10].ammo = 2;
    weapon_base[10].fireRate = 1800;
    weapon_base[10].velocity = 0.04;
    weapon_base[10].projLifespan = 5000;
    weapon_base[10].damage = 100;
    weapon_base[10].decayRate = 0.9997; //0.7408/s
    weapon_base[10].deviation = TO_RAD(180);
    weapon_base[10].bulletCount = 300;
    weapon_base[10].projGravityF = 0;
    //weapon 11
    projectile_base[4].t = loadTexture("P_purplebullet.png", 255, 255, 255);
    projectile_base[4].hitbox = 0.35;
    weapon_base[11].t = loadTexture("W_InYourFace.png", 255, 255, 255);
    weapon_base[11].name = "In Your Face?";
    weapon_base[11].projType = 4; //purple bullet
    weapon_base[11].ammo = 12;
    weapon_base[11].fireRate = 700;
    weapon_base[11].velocity = 0.04;
    weapon_base[11].projLifespan = 8000;
    weapon_base[11].damage = 200;
    weapon_base[11].decayRate = 0.991; //0.0001185/s
    weapon_base[11].deviation = TO_RAD(2);
    weapon_base[11].bulletCount = 20;
    weapon_base[11].projGravityF = 0.2;
    //weapon 12
    projectile_base[5].t = loadTexture("P_greenbullet.png", 255, 255, 255);
    projectile_base[5].hitbox = 0.35;
    weapon_base[12].t = loadTexture("W_Ghostcannon.png", 255, 255, 255);
    weapon_base[12].name = "Ghost Cannon";
    weapon_base[12].projType = 5; //green bullet
    weapon_base[12].ammo = 13;
    weapon_base[12].fireRate = 900;
    weapon_base[12].velocity = 0.02;
    weapon_base[12].projLifespan = 8000;
    weapon_base[12].damage = 70;
    weapon_base[12].decayRate = 1; //1.000/s
    weapon_base[12].deviation = TO_RAD(10);
    weapon_base[12].bulletCount = 25;
    weapon_base[12].projGravityF = 0;
    weapon_base[12].spectral = true;
    //weapon 13
    weapon_base[13].t = loadTexture("W_Guanoballv2.png", 255, 255, 255);
    weapon_base[13].name = "GBall v2";
    weapon_base[13].projType = 1; //white bullet
    weapon_base[13].ammo = 750;
    weapon_base[13].fireRate = 10;
    weapon_base[13].velocity = 0.002;
    weapon_base[13].projLifespan = 8000;
    weapon_base[13].damage = 50;
    weapon_base[13].decayRate = 0.9998; //0.8187/s
    weapon_base[13].deviation = TO_RAD(180);
    weapon_base[13].bulletCount = 1;
    weapon_base[13].projGravityF = -1;
    //weapon 14
    weapon_base[14].t = loadTexture("W_J300.png", 255, 255, 255);
    weapon_base[14].name = "J300";
    weapon_base[14].projType = 1; //white bullet
    weapon_base[14].ammo = 30;
    weapon_base[14].fireRate = 250;
    weapon_base[14].velocity = 0.05;
    weapon_base[14].projLifespan = 8000;
    weapon_base[14].damage = 210;
    weapon_base[14].decayRate = 0.9998; //0.8187/s
    weapon_base[14].deviation = TO_RAD(0.5);
    weapon_base[14].bulletCount = 1;
    weapon_base[14].projGravityF = 0;
    //weapon 15
    weapon_base[15].t = loadTexture("W_SpectralRifle.png", 255, 255, 255);
    weapon_base[15].name = "Spectral Rifle";
    weapon_base[15].projType = 5; //green bullet
    weapon_base[15].ammo = 25;
    weapon_base[15].fireRate = 270;
    weapon_base[15].velocity = 0.05;
    weapon_base[15].projLifespan = 8000;
    weapon_base[15].damage = 180;
    weapon_base[15].decayRate = 0.9998; //0.8187/s
    weapon_base[15].deviation = TO_RAD(0.5);
    weapon_base[15].bulletCount = 1;
    weapon_base[15].projGravityF = 0;
    weapon_base[15].spectral = true;
    //weapon 16
    projectile_base[6].t = loadTexture("P_bluebomb.png", 255, 255, 255);
    projectile_base[6].hitbox = 0.2;
    weapon_base[16].t = loadTexture("W_bomber.png", 255, 255, 255);
    weapon_base[16].name = "Bomber";
    weapon_base[16].projType = 6; //blue bomb
    weapon_base[16].ammo = 14;
    weapon_base[16].fireRate = 800;
    weapon_base[16].velocity = 0.05;
    weapon_base[16].projLifespan = 100000;
    weapon_base[16].damage = 700;
    weapon_base[16].decayRate = 1; //1.000/s
    weapon_base[16].deviation = TO_RAD(3);
    weapon_base[16].bulletCount = 1;
    weapon_base[16].projGravityF = 0.1;
    weapon_base[16].explosionRadius = 6;
    weapon_base[16].explosionDuration = 2000;
    //weapon 17
    weapon_base[17].t = loadTexture("W_boomcannon.png", 255, 255, 255);
    weapon_base[17].name = "Boom Cannon";
    weapon_base[17].projType = 6; //blue bomb
    weapon_base[17].ammo = 8;
    weapon_base[17].fireRate = 800;
    weapon_base[17].velocity = 0.04;
    weapon_base[17].projLifespan = 8000;
    weapon_base[17].damage = 300;
    weapon_base[17].decayRate = 1; //1.000/s
    weapon_base[17].deviation = TO_RAD(10);
    weapon_base[17].bulletCount = 10;
    weapon_base[17].projGravityF = 0.2;
    weapon_base[17].explosionRadius = 3;
    weapon_base[16].explosionDuration = 1200;
    //weapon 18
    projectile_base[7].t = loadTexture("P_nothing.png", 255, 255, 255);
    projectile_base[7].hitbox = 0.2;
    weapon_base[18].t = loadTexture("W_ringofflame.png", 255, 255, 255);
    weapon_base[18].name = "Ring of Flame";
    weapon_base[18].projType = 7; //nothing
    weapon_base[18].ammo = 1600;
    weapon_base[18].fireRate = 2;
    weapon_base[18].velocity = 0.1;
    weapon_base[18].projLifespan = 50;
    weapon_base[18].damage = 120;
    weapon_base[18].decayRate = 1; //1.000/s
    weapon_base[18].deviation = TO_RAD(180);
    weapon_base[18].bulletCount = 1;
    weapon_base[18].projGravityF = 0;
    weapon_base[18].explosionRadius = 1;
    weapon_base[18].explosionDuration = -200;
    weapon_base[18].spectral = true;
    //weapon 19
    weapon_base[19].t = loadTexture("W_NukeCannon.png", 255, 255, 255);
    weapon_base[19].name = "Nuke Cannon";
    weapon_base[19].projType = 6; //blue bomb
    weapon_base[19].ammo = 4;
    weapon_base[19].fireRate = 1600;
    weapon_base[19].velocity = 0.06;
    weapon_base[19].projLifespan = 8000;
    weapon_base[19].damage = 3000;
    weapon_base[19].decayRate = 1; //1.000/s
    weapon_base[19].deviation = TO_RAD(1);
    weapon_base[19].bulletCount = 1;
    weapon_base[19].projGravityF = 0.1;
    weapon_base[19].explosionRadius = 15;
    weapon_base[19].explosionDuration = 2500;
    //weapon 20
    weapon_base[20].t = loadTexture("W_AutoBoom.png", 255, 255, 255);
    weapon_base[20].name = "Auto Boom";
    weapon_base[20].projType = 6; //blue bomb
    weapon_base[20].ammo = 25;
    weapon_base[20].fireRate = 200;
    weapon_base[20].velocity = 0.06;
    weapon_base[20].projLifespan = 8000;
    weapon_base[20].damage = 400;
    weapon_base[20].decayRate = 1; //1.000/s
    weapon_base[20].deviation = TO_RAD(1);
    weapon_base[20].bulletCount = 1;
    weapon_base[20].projGravityF = 0.1;
    weapon_base[20].explosionRadius = 4;
    weapon_base[20].explosionDuration = 1500;
    //weapon 21
    projectile_base[8].t = loadTexture("P_purplebomb.png", 255, 255, 255);
    projectile_base[8].hitbox = 0.2;
    weapon_base[21].t = loadTexture("W_Ghostboom.png", 255, 255, 255);
    weapon_base[21].name = "Peek A BOOM";
    weapon_base[21].projType = 8; //purple spectral bomb
    weapon_base[21].ammo = 8;
    weapon_base[21].fireRate = 300;
    weapon_base[21].velocity = 0.03;
    weapon_base[21].projLifespan = 10000;
    weapon_base[21].damage = 200;
    weapon_base[21].decayRate = 1; //1.000/s
    weapon_base[21].deviation = TO_RAD(20);
    weapon_base[21].bulletCount = 20;
    weapon_base[21].projGravityF = 0;
    weapon_base[21].explosionRadius = 3;
    weapon_base[21].explosionDuration = 1200;
    weapon_base[21].spectral = true;
    //weapon 22
    weapon_base[22].t = loadTexture("W_seeyoulater.png", 255, 255, 255);
    weapon_base[22].name = "See You Later";
    weapon_base[22].projType = 6; //blue bomb
    weapon_base[22].ammo = 1;
    weapon_base[22].fireRate = 2500;
    weapon_base[22].velocity = 0.02;
    weapon_base[22].projLifespan = 10000;
    weapon_base[22].damage = 900;
    weapon_base[22].decayRate = 1; //1.000/s
    weapon_base[22].deviation = TO_RAD(180);
    weapon_base[22].bulletCount = 200;
    weapon_base[22].projGravityF = 0;
    weapon_base[22].explosionRadius = 3;
    weapon_base[22].explosionDuration = -1200;
    weapon_base[22].spectral = true;
}
