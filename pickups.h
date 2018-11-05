const int PICKUP_TYPES = 4;
const int BASE_PICKUP_SPAWN_TIME = 2000; //milliseconds delay from when a pickup is known to spawn somewhere to when it actually spawns there
texture *t_pickup[PICKUP_TYPES];
struct pickup
{
    int type;
    int x, y;
    int waitTime;
    int spawnPoint;
    pickup()
    {
        waitTime = BASE_PICKUP_SPAWN_TIME;
        double z = randf(); //randomly determine what type of pickup this is
        type = z*PICKUP_TYPES/2 - 0.01; //let's say only pickups 0 and 1 can spawn
        int a;
        do
        {
            a = randuz() % gameMaps[currentMap].pickupSpawnPoints.size();
            x = gameMaps[currentMap].pickupSpawnPoints[a].A;
            y = gameMaps[currentMap].pickupSpawnPoints[a].B;
        }
        while(pickup_spawn_occupied[a]);
        pickup_spawn_occupied[a] = true;
        spawnPoint = a;
    }
    void render()
    {
        if(waitTime == 0) //pickup has spawned
            renderCopy(t_pickup[type], x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE);
        else //pickup hasn't spawned yet
        {
            fillCircle((x+0.5)*BLOCK_SIZE, (y+0.5)*BLOCK_SIZE, BLOCK_SIZE, 255, 0, 0, 30 + 80 * (1 - waitTime/(double)BASE_PICKUP_SPAWN_TIME));
            waitTime -= getFrameLength();
            if(waitTime < 0)
                waitTime = 0;
        }
    }
    bool intersects(double x, double y, double add=0)
    {
        for(auto &i: pickups)
        {
            if(rectsIntersect({x*BLOCK_SIZE, y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE}, {i.x*BLOCK_SIZE, i.y*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE}))
            {
                return true;
            }
        }
        return false;
    }
};
void processPickups()
{
    int cnt = 0; //how many pickup spawn points aren't occupied?
    for(int i=0; i<PICKUP_SPAWN_POINTS; i++)
    {
        if(!pickup_spawn_occupied[i])
        {
            cnt++;
        }
    }
    if(cnt > 0)
    {
        for(int i=0; cnt>0 && i<getFrameLength(); i++)
        {
            if(randf()/NUM_PLAYERS < PICKUP_SPAWN_RATE) //pickup spawn rate is directly proportional to number of players
            {
                pickups.push_back({});
                cnt--;
            }
        }
    }
    for(auto &i: pickups)
        i.render();
}
