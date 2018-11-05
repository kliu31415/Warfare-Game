vector<int> menu_select; //which box is selected in the menu
int indiv_selected; //used to denote player number in controls
enum class menuScreen{mainMenu, controls, options, versionInfo, indivControl, video, gameSettings};
vector<menuScreen> cur_menu; //which menu is currently being used
int MENU_W_OFFSET, MENU_H_OFFSET, MENU_W;
void nextMenu(menuScreen x) //deeper nesting (Ex. main->options)
{
    menu_select.push_back(0);
    cur_menu.push_back(x);
}
void prevMenu() //going back (Ex. options->main)
{
    menu_select.pop_back();
    cur_menu.pop_back();
}
struct menu_option
{
    void (*func)();
    virtual void inc() = 0;
    virtual void dec() = 0;
    virtual void reset() = 0;
    virtual void drawInfo(int x, int y, int s) = 0;
};
template<class T> struct T_menu_option: public menu_option
{
    string name;
    T minVal, maxVal, baseVal, interval;
    T *curVal;
    double mult; //sometimes we should multiply the actual value to display a more friendly user value
    T_menu_option(string n, T *c, T basev, T minv, T maxv, T i, double m, void (*f)())
    {
        name = n;
        curVal = c;
        minVal = minv;
        maxVal = maxv;
        baseVal = basev;
        mult = m;
        interval = i;
        func = f;
    }
    void inc()
    {
        if(interval > 0) //BASE_JUMP_VELOCITY<0 causes some issues
            *curVal = min((T)(*curVal + interval), maxVal);
        else *curVal = max((T)(*curVal + interval), minVal);
        if(func != NULL)
            func();
    }
    void dec()
    {
        if(interval > 0)
            *curVal = max((T)(*curVal - interval), minVal);
        else *curVal = min((T)(*curVal - interval), maxVal);
        if(func != NULL)
            func();
    }
    void reset()
    {
        *curVal = baseVal;
        if(func != NULL)
            func();
    }
    void drawInfo(int x, int y, int s)
    {
        drawText(name, x, y, s);
        drawText(to_str(round(mult * (*curVal), 2)), MENU_W_OFFSET + MENU_W + settings::WINDOW_W*0.02, y, s);
    }
};
struct T_bool_menu_option: public menu_option
{
    string name;
    bool *curVal;
    T_bool_menu_option(string n, bool *c, void (*f)())
    {
        name = n;
        curVal = c;
        func = f;
    }
    void inc(){}
    void dec(){}
    void reset()
    {
        *curVal = !*curVal;
        if(func != NULL)
            func();
    }
    void drawInfo(int x, int y, int s)
    {
        drawText(name, x, y, s);
        if(*curVal)
            drawText(to_str("TRUE"), settings::WINDOW_W*0.62, y, s);
        else drawText(to_str("FALSE"), settings::WINDOW_W*0.62, y, s);
    }
};
struct menu_screen
{
    string title;
    vector<string> info; //useful info that is displayed at the bottom
    vector<menu_option*> options;
    void render()
    {
        using namespace settings;
        int H = WINDOW_H, W = WINDOW_W;
        renderClear(100, 100, 100);
        drawText(title, W*0.4, H*0.1, H*0.1);
        fillRect(MENU_W_OFFSET - W*0.01, H*0.19, MENU_W + WINDOW_W*0.02, H*(0.02 + 0.05*(options.size()+1)), 150, 150, 150); //outer box
        fillRect(MENU_W_OFFSET, H*0.2, MENU_W, H*0.05*(options.size()+1), 200, 200, 200); //inner box
        fillRect(MENU_W_OFFSET, H*(0.2 + 0.05*menu_select.back()), MENU_W, H*0.05 + 1, 200, 150, 100); //highlight the selected option
        setColor(0, 0, 0);
        double hOffset = H*0.2;
        for(auto &i: options)
        {
            i->drawInfo(MENU_W_OFFSET, hOffset, H*0.05);
            hOffset += H*0.05;
            drawLine(MENU_W_OFFSET, hOffset, MENU_W_OFFSET + MENU_W, hOffset);
        }
        drawText("BACK", MENU_W_OFFSET, H*(0.2 + 0.05*options.size()), H*0.05);
        for(int i=0; i<info.size(); i++)
            drawText(info[i], W*0.5 - (H*0.0125*info[i].size()), H*(0.26 + 0.05*(options.size() + i)), H*0.05);
    }
    void inc(int pos)
    {
        options[pos]->inc();
    }
    void dec(int pos)
    {
        options[pos]->dec();
    }
    void reset(int pos)
    {
        options[pos]->reset();
    }
};
menu_screen gameSettingsMenu, videoMenu;
template<class T> void addMenuOption(vector<menu_option*> &o, string n, T *c, T basev, T minv, T maxv, T i, double m=1, void (*f)() = NULL)
{
    o.push_back(new T_menu_option<T>(n, c, basev, minv, maxv, i, m, f));
}
void addBoolMenuOption(vector<menu_option*> &o, string n, bool *c, void (*f)() = NULL)
{
    o.push_back(new T_bool_menu_option(n, c, f));
}
void initMenu()
{
    using namespace settings;
    currentState = gameState::menu;
    menu_select.clear();
    menu_select.push_back(0);
    cur_menu.clear();
    cur_menu.push_back(menuScreen::mainMenu);
    static bool alreadyInit = false;
    if(!alreadyInit)
    {
    MENU_W_OFFSET = WINDOW_W * 0.3;
    MENU_H_OFFSET = WINDOW_H * 0.1;
    MENU_W = WINDOW_W * 0.4;
    alreadyInit =true;
    {//game settings menu
    gameSettingsMenu.title = "SETTINGS";
    gameSettingsMenu.info.push_back("PRESS ENTER TO RESET A FIELD");
    vector<menu_option*> &o = gameSettingsMenu.options;
    addMenuOption(o, "GRAVITY", &BASE_GRAVITY, 0.0001, -0.0005, 0.0005, BASE_GRAVITY/10, 1/BASE_GRAVITY);
    addMenuOption(o, "MOVEMENT SPEED", &BASE_MOVE_SPEED, 0.01, 0.0, 0.05, BASE_MOVE_SPEED/10, 1/BASE_MOVE_SPEED);
    addMenuOption(o, "AIR RESISTANCE", &BASE_AIR_RESISTANCE, 7e-6, -5e-5, 5e-5, BASE_AIR_RESISTANCE/20, 1/BASE_AIR_RESISTANCE);
    addMenuOption(o, "JUMP VELOCITY", &BASE_JUMP_VELOCITY, -0.03, -0.1, 0.1, BASE_JUMP_VELOCITY/20, 1/BASE_JUMP_VELOCITY);
    addMenuOption(o, "FALL RESISTANCE", &BASE_FALLING_INJURY_F, 0.05, 0.0, 0.2, BASE_FALLING_INJURY_F/20, 1/BASE_FALLING_INJURY_F);
    addMenuOption(o, "FIRE DELAY (MS)", &FIRE_DELAY, 30, 0, 100, 1);
    addMenuOption(o, "TICKS/MS", &INTERVAL_MOD, 1.0, 0.05, 20.0, INTERVAL_MOD/20);
    addMenuOption(o, "GAME LENGTH", &GAME_LENGTH, 300, 15, 3600, GAME_LENGTH/20);
    addMenuOption(o, "ITEM SPAWN RATE", &PICKUP_SPAWN_RATE, 0.0002, 0.0, 0.001, PICKUP_SPAWN_RATE/20, 1/PICKUP_SPAWN_RATE);
    addMenuOption(o, "# PLAYERS", &NUM_PLAYERS, 2, 1, 4, 1);
    addMenuOption(o, "MAP WIDTH", &MAP_W, 40, 16, 120, 1);
    addMenuOption(o, "MAP HEIGHT", &MAP_H, 25, 10, 75, 1);
    }
    {//video menu
    videoMenu.title = "VIDEO";
    videoMenu.info.push_back("SOME CHANGES MAY REQUIRE RESTARTING");
    videoMenu.info.push_back("2:1 WINDOW_W/WINDOW_H RATIO IS OPTIMAL");
    vector<menu_option*> &o = videoMenu.options;
    addBoolMenuOption(o, "LOW TEXTURE QUALITY", &lowTextureQuality, reinit);
    addBoolMenuOption(o, "VSYNC", &vsync, reinit);
    addBoolMenuOption(o, "ACCELERATED RENDERER", &acceleratedRenderer, reinit);
    addBoolMenuOption(o, "FULLSCREEN", &IS_FULLSCREEN, reinit);
    addBoolMenuOption(o, "SHOW FPS", &showFPS);
    addMenuOption(o, "FPS CAP", &FPS_CAP, 300, 1, 300, 1);
    addMenuOption(o, "BRIGHTNESS", &brightness, brightness, 0.0, 1.0, 0.05, 1, resetBrightness);
    addMenuOption(o, "R GAMMA", &Rgamma, Rgamma, 0.1, 4.0, 0.05, 1, resetGamma);
    addMenuOption(o, "G GAMMA", &Ggamma, Ggamma, 0.1, 4.0, 0.05, 1, resetGamma);
    addMenuOption(o, "B GAMMA", &Bgamma, Bgamma, 0.1, 4.0, 0.05, 1, resetGamma);
    addMenuOption(o, "WINDOW WIDTH", &FOUT_WINDOW_W, 1500, 100, 4000, 10);
    addMenuOption(o, "WINDOW HEIGHT", &FOUT_WINDOW_H, 750, 50, 2000, 10);
    }
    }
}
