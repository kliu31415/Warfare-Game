int currentMap = 0;
int BLOCK_SIZE = settings::WINDOW_H/MAP_H;
int PICKUP_SPAWN_POINTS = 30;
texture *t_dirt1 = NULL, *t_rock1 = NULL;
int baseBlockHP[3]{0, 100, 1e9};
struct gameMap
{
    enum class block{empty, dirt1, rock1, timer};
    block blocks[MAX_MAP_W][MAX_MAP_H];
    int blockHP[MAX_MAP_W][MAX_MAP_H];
    vector<pii> playerSpawnPoints, pickupSpawnPoints;
    void render()
    {
        for(int i=0; i<MAP_W; i++)
        {
            for(int j=0; j<MAP_H; j++)
            {
                switch(blocks[i][j])
                {
                case block::empty:
                    //fillRect(i*BLOCK_SIZE, j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, 0, 0, 0); black is empty anyway
                    break;
                case block::dirt1:
                    printf("Error: Asked to render a dirt block, but those don't exist\n");
                    //renderCopy(t_dirt1, i*BLOCK_SIZE, j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    break;
                case block::rock1:
                case block::timer:
                    if(settings::lowTextureQuality)
                        fillRect(i*BLOCK_SIZE, j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, 195, 195, 195);
                    else renderCopy(t_rock1, i*BLOCK_SIZE, j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
                    break;
                default:
                    printf("Error: Asked to render an unknown type of block\n");
                    break;
                }
            }
        }
    }
};
vector<gameMap> gameMaps;
namespace verifyMap
{
    int p[MAX_MAP_W * MAX_MAP_H], sz[MAX_MAP_W * MAX_MAP_H];
    int getp(int x)
    {
        if(x == p[x])
            return x;
        p[x] = getp(p[x]);
        return p[x];
    }
    void combine(int a, int b)
    {
        a = getp(a);
        b = getp(b);
        if(a == b)
            return;
        sz[b] += sz[a];
        sz[a] = 0;
        p[a] = b;
    }
    void verify(gameMap &x)
    {
        iota(p, end(p), 0);
        fill(sz, end(sz), 1);
        for(int i=0; i<MAP_W; i++)
        {
            for(int j=0; j<MAP_H; j++)
            {
                if(i != MAP_W-1)
                {
                    if(x.blocks[i][j]==gameMap::block::empty && x.blocks[i+1][j]==gameMap::block::empty)
                        combine(i*MAP_H + j, (i+1)*MAP_H + j);
                }
                if(j != MAP_H-1)
                {
                    if(x.blocks[i][j]==gameMap::block::empty && x.blocks[i][j+1]==gameMap::block::empty)
                        combine(i*MAP_H + j, i*MAP_H + j+1);
                }
            }
        }
        int biggestGroup = max_element(sz, end(sz)) - sz;
        for(int i=0; i<MAP_W; i++)
        {
            for(int j=0; j<MAP_H; j++)
            {
                if(x.blocks[i][j]==gameMap::block::empty && getp(i*MAP_H + j)!=biggestGroup)
                    x.blocks[i][j] = gameMap::block::rock1;
            }
        }
    }
}
bool intersectsBlocks(double x, double y)
{
    for(int i=max(int(x-1), 0); i<=min(int(x+1), MAP_W-1); i++)
    {
        for(int j=max(int(y-1), 0); j<=min(int(y+1), MAP_H-1); j++)
        {
            if(gameMaps[currentMap].blocks[i][j] == gameMap::block::empty) //if this cell is empty we don't care about collisions
                continue;
            if(rectsIntersect({(x+0.15)*BLOCK_SIZE, (y+0.15)*BLOCK_SIZE, BLOCK_SIZE*0.7, BLOCK_SIZE*0.7},
                                {i*BLOCK_SIZE, j*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE})) //checks if the player intersects with the block
                {
                    return true;
            }
        }
    }
    return false;
}
bool pickup_spawn_occupied[100]; //0 if the pickup spawn point is occupied
void initMaps()
{
    fill(pickup_spawn_occupied, end(pickup_spawn_occupied), false);
    BLOCK_SIZE = settings::WINDOW_H / MAP_H;
    //MAP 1
    gameMaps.push_back({});
    gameMap &x = gameMaps.back();
    for(int i=0; i<MAP_W; i++)
        x.blocks[i][0] = x.blocks[i][MAP_H-1] = gameMap::block::rock1;
    for(int i=0; i<MAP_H; i++)
        x.blocks[0][i] = x.blocks[MAP_W-1][i] = gameMap::block::rock1;
    for(int i=1; i<MAP_H-1; i++)
    {
        int streak = 0; //we want longer horizontal platforms
        for(int j=1; j<MAP_W-1; j++)
        {
            if(i!=1 && x.blocks[j][i-2]==gameMap::block::rock1 && x.blocks[j][i-1]==gameMap::block::empty)
            {
                x.blocks[j][i] = gameMap::block::empty; //corridors 1 block tall are just annoying to get through so don't let them exist
                streak = 0;
                continue;
            }
            int v = randuz() % 64;
            if(v <= min(6 + (int)(15*pow(streak, 0.7)), 50))
            {
                x.blocks[j][i] = gameMap::block::rock1;
                streak++;
            }
            else
            {
                x.blocks[j][i] = gameMap::block::empty;
                streak = 0;
            }
        }
    }
    for(int i=0.45*MAP_W; i<=0.55*MAP_W; i++)
    {
        for(int j=0.45*MAP_H; j<=0.55*MAP_H; j++)
        {
            x.blocks[i][j] = gameMap::block::timer;
        }
    }
    for(int i=1; i<MAP_W-1; i++) //get rid of some isolated 1x1 blocks to make the map prettier
    {
        for(int j=1; j<MAP_H-1; j++)
        {
            if(x.blocks[i][j]==gameMap::block::rock1 && x.blocks[i][j+1]==gameMap::block::empty && x.blocks[i][j-1]==gameMap::block::empty
               && x.blocks[i+1][j]==gameMap::block::empty && x.blocks[i-1][j]==gameMap::block::empty)
            {
                if(randf() < 0.8)
                    x.blocks[i][j] = gameMap::block::empty;
            }
        }
    }
    verifyMap::verify(x);
    while(x.playerSpawnPoints.size() < 30)
    {
        int a = 1 + randuz()%(MAP_W-2), b = 1 + randuz()%(MAP_H-2);
        if(x.blocks[a][b] != gameMap::block::empty || (x.blocks[a][b+1] != gameMap::block::rock1 && x.blocks[a][b+1] != gameMap::block::timer))
            continue;
        bool bad = false;
        for(auto &j: x.playerSpawnPoints)
        {
            if(j == mp(a, b))
            {
                bad = true;
                break;
            }
        }
        if(!bad)
            x.playerSpawnPoints.push_back(mp(a, b));
    }
    while(x.pickupSpawnPoints.size() < PICKUP_SPAWN_POINTS)
    {
        int a = 1 + rand()%(MAP_W-2), b = 1 + rand()%(MAP_H-2);
        if(x.blocks[a][b] != gameMap::block::empty || (x.blocks[a][b+1] != gameMap::block::rock1 && x.blocks[a][b+1] != gameMap::block::timer))
            continue;
        bool bad = false;
        for(auto &j: x.playerSpawnPoints)
        {
            if(j == mp(a, b))
            {
                bad = true;
                break;
            }
        }
        for(auto &j: x.pickupSpawnPoints)
        {
            if(j == mp(a, b))
            {
                bad = true;
                break;
            }
        }
        if(!bad)
            x.pickupSpawnPoints.push_back(mp(a, b));
    }
    for(int i=0; i<MAP_W; i++)
    {
        for(int j=0; j<MAP_H; j++)
        {
            x.blockHP[i][j] = baseBlockHP[(int)x.blocks[i][j]];
        }
    }
}
