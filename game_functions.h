double BASE_GRAVITY = 0.0001;
double BASE_AIR_RESISTANCE = 0.000007;
double BASE_JUMP_VELOCITY = -0.03;
double BASE_MOVE_SPEED = 0.01;
double RESPAWN_TIME = 5000; //milliseconds
double BASE_FALLING_INJURY_F = 0.05;
double BASE_GOLD_STOLEN = 0.1; //proportion of gold stolen on a kill
double PICKUP_SPAWN_RATE = 0.0002; //average number of pickups spawning per millisecond (multiplied by number of players)
int STARTING_GOLD = 1000; //amount of gold everyone starts with
int GAME_LENGTH = 300; //seconds
double INTERVAL_MOD = 1; //how many times things are moved per millisecond. Higher = lower performance but more accurate.
int NUM_PLAYERS = 2;
const int MAX_PLAYERS = 4;
const int NUM_SHOP_ITEMS = 10;
int MAP_W = 80, MAP_H = 50;
const int MAX_MAP_W = 80, MAX_MAP_H = 50;
int FIRE_DELAY = 30; //milliseconds before waiting to enable smoother diagonal firing
SDL_Scancode SDL_SCANCODE_NOT_ASSIGNED = SDL_SCANCODE_KP_XOR; //nobody has a keypad xor key probably
void loadItems();
void reinit()
{
    reinitSDL();
    loadItems();
}
void quit_game()
{
    using namespace settings;
    sdl_settings::output_config();
    SDL_Quit();
    exit(EXIT_SUCCESS);
}
struct disappearingText
{
    texture *text;
    int duration; //milliseconds
    int x, y, w, h;
    double dAlpha; //alpha change per second
    uint8_t a;
    disappearingText(string s, int _x, int _y, int sz, int d, double dA, uint8_t r=0, uint8_t g=0, uint8_t b=0, uint8_t _a=255)
    {
        a = _a;
        dAlpha = dA;
        x = _x;
        y = _y;
        w = sz * s.size()/2;
        h = sz;
        duration = d;
        text = createText(s, sz, r, g, b, a);
        if(a != 255)
            SDL_SetTextureAlphaMod(text, a);
    }
};
vector<disappearingText> disapText;
void handleText()
{
    for(int i=0; i<disapText.size(); i++)
    {
        disappearingText &x = disapText[i];
        x.duration -= getFrameLength();
        double dA = x.dAlpha * getFrameLength() / 1000;
        if(x.duration<=0 || x.a<-dA) //time to stop showing this text (duration expired or alpha<0)
        {
            disapText.erase(disapText.begin() + i);
            i--;
            continue;
        }
        x.a += dA;
        SDL_SetTextureAlphaMod(x.text, x.a);
        renderCopy(x.text, x.x, x.y, x.w, x.h);
    }
}
