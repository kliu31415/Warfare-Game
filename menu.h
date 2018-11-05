#include "menu_base.h"
void initGame();
void operateIndivControlMenu();
void initPregame();
void operateMainMenu()
{
    const static int MENU_OPTIONS = 5;
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
                menu_select.back() += MENU_OPTIONS-1;
                menu_select.back() %= MENU_OPTIONS;
                break;
            case SDLK_DOWN:
                menu_select.back()++;
                menu_select.back() %= MENU_OPTIONS;
                break;
            case SDLK_RETURN:
                switch(menu_select.back())
                {
                case 0:
                    initPregame();
                    break;
                case 1:
                    nextMenu(menuScreen::controls);
                    break;
                case 2:
                    nextMenu(menuScreen::options);
                    break;
                case 3:
                    nextMenu(menuScreen::versionInfo);
                    break;
                case 4:
                    quit_game();
                    break;
                }
                break;
            case SDLK_ESCAPE:
                quit_game();
                break;
            }
            break;
        }
    }
    int H = settings::WINDOW_H, W = settings::WINDOW_W;
    renderClear(100, 100, 100);
    drawText("MAIN MENU", MENU_W_OFFSET, H*0.1, H*0.1);
    fillRect(MENU_W_OFFSET - W*0.01, H*0.19, MENU_W + W*0.02, H*(0.02 + 0.05*MENU_OPTIONS), 150, 150, 150); //outer box
    fillRect(MENU_W_OFFSET, H*0.2, MENU_W, H*0.05*MENU_OPTIONS, 200, 200, 200); //inner box
    fillRect(MENU_W_OFFSET, H*(0.2 + 0.05*menu_select.back()), MENU_W, H*0.05 + 1, 200, 150, 100); //highlight the selected option
    setColor(0, 0, 0);
    for(int i=1; i<MENU_OPTIONS; i++)
        drawLine(MENU_W_OFFSET, H*(0.2 + i*0.05), MENU_W_OFFSET + MENU_W, H*(0.2 + i*0.05));
    drawText("PLAY", MENU_W_OFFSET, H*0.2, H*0.05); //option 0
    drawText("CONTROLS", MENU_W_OFFSET, H*0.25, H*0.05); //option 1
    drawText("OPTIONS", MENU_W_OFFSET, H*0.3, H*0.05); //option 2
    drawText("VERSION INFO", MENU_W_OFFSET, H*0.35, H*0.05); //option 3
    drawText("QUIT", MENU_W_OFFSET, H*0.4, H*0.05); //option 4
}
void operateControlMenu()
{
    const static int MENU_OPTIONS = 5;
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
                menu_select.back() += MENU_OPTIONS-1;
                menu_select.back() %= MENU_OPTIONS;
                break;
            case SDLK_DOWN:
                menu_select.back()++;
                menu_select.back() %= MENU_OPTIONS;
                break;
            case SDLK_RETURN:
                switch(menu_select.back())
                {
                case 0:
                case 1:
                case 2:
                case 3:
                    indiv_selected = menu_select.back();
                    nextMenu(menuScreen::indivControl);
                    break;
                case 4:
                    prevMenu();
                    break;
                }
                break;
            case SDLK_ESCAPE:
                prevMenu();
                break;
            }
            break;
        }
    }
    int H = settings::WINDOW_H, W = settings::WINDOW_W;
    renderClear(100, 100, 100);
    drawText("CONTROLS", MENU_W_OFFSET, H*0.1, H*0.1);
    fillRect(MENU_W_OFFSET - W*0.01, H*0.19, MENU_W + W*0.02, H*(0.02 + 0.05*MENU_OPTIONS), 150, 150, 150); //outer box
    fillRect(MENU_W_OFFSET, H*0.2, MENU_W, H*0.05*MENU_OPTIONS, 200, 200, 200); //inner box
    fillRect(MENU_W_OFFSET, H*(0.2 + 0.05*menu_select.back()), MENU_W, H*0.05 + 1, 200, 150, 100); //highlight the selected option
    setColor(0, 0, 0);
    for(int i=1; i<MENU_OPTIONS; i++)
        drawLine(MENU_W_OFFSET, H*(0.2 + i*0.05), MENU_W + MENU_W_OFFSET, H*(0.2 + i*0.05));
    drawText("PLAYER 1", MENU_W_OFFSET, H*0.2, H*0.05); //option 0
    drawText("PLAYER 2", MENU_W_OFFSET, H*0.25, H*0.05); //option 1
    drawText("PLAYER 3", MENU_W_OFFSET, H*0.3, H*0.05); //option 2
    drawText("PLAYER 4", MENU_W_OFFSET, H*0.35, H*0.05); //option 3
    drawText("BACK", MENU_W_OFFSET, H*0.4, H*0.05); //option 4
}
namespace versionInfo
{
    string compilerName;
    string compilerVersion;
    string operatingSystem;
    string sdl2CompileVersion;
    string sdl2ImageCompileVersion;
    string sdl2TTFCompileVersion;
    string fontName;
    string gameVersion;
    string cppVersion;
    string displayDPI;
    void init()
    {
        //get compiler name and version
        #ifdef __MINGW64__
        compilerName = "MinGW64";
        compilerVersion = __VERSION__;
        #elif __MINGW32__
        compilerName = "MinGW32";
        compilerVersion = __VERSION__;
        #elif _MSC_VER
        compilerName = "MSVC";
        compilerVersion = to_str(_MSC_VER);
        #elif __GCC__
        compilerName = "GCC";
        compilerVersion = __VERSION__;
        #else
        compilerName = "(Unknown Compiler)";
        compilerVersion = "(Unknown Version)";
        #endif
        //get operating system
        #ifdef __linux__
        operatingSystem = "Linux";
        #elif _WIN32
        operatingSystem = "Windows32";
        #elif _WIN64
        operatingSystem = "Windows64";
        #elif __unix__
        operatingSystem = "Unix";
        #elif __APPLE__
        operatingSystem = "Mac OSX";
        #else
        operating System = "Unknown";
        #endif
        //get SDL2 version
        SDL_version cV;
        SDL_VERSION(&cV);
        sdl2CompileVersion = to_str((int)cV.major) + "." + to_str((int)cV.minor) + "." + to_str((int)cV.patch);
        SDL_IMAGE_VERSION(&cV);
        sdl2ImageCompileVersion = to_str((int)cV.major) + "." + to_str((int)cV.minor) + "." + to_str((int)cV.patch);
        TTF_VERSION(&cV);
        sdl2TTFCompileVersion = to_str((int)cV.major) + "." + to_str((int)cV.minor) + "." + to_str((int)cV.patch);
        //get font name
        fontName = to_str(TTF_FontFaceFamilyName(getFont(0))) + " " + to_str(TTF_FontFaceStyleName(getFont(0)));
        //get game version
        gameVersion = to_str(__GAME__VERSION__);
        //get C++ version
        cppVersion = to_str((int)__cplusplus);
        //get DPI
        int displayIndex = SDL_GetWindowDisplayIndex(getWindow());
        float dpi;
        SDL_GetDisplayDPI(displayIndex, &dpi, &dpi, &dpi);
        displayDPI = to_str((int)dpi);
    }
}
void operateVersionInfoMenu()
{
    const static int MENU_OPTIONS = 1;
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
            case SDLK_RETURN:
                prevMenu();
                break;
            case SDLK_ESCAPE:
                prevMenu();
                break;
            }
            break;
        }
    }
    int H = settings::WINDOW_H, W = settings::WINDOW_W;
    renderClear(100, 100, 100);
    drawText("VERSION INFO", MENU_W_OFFSET, H*0.1, H*0.1);
    fillRect(MENU_W_OFFSET - W*0.01, H*0.19, MENU_W + W*0.02, H*(0.02 + 0.05*MENU_OPTIONS), 150, 150, 150); //outer box
    fillRect(MENU_W_OFFSET, H*0.2, MENU_W, H*0.05*MENU_OPTIONS, 200, 200, 200); //inner box
    fillRect(MENU_W_OFFSET, H*(0.2 + 0.05*menu_select.back()), MENU_W, H*0.05 + 1, 200, 150, 100); //highlight the selected option
    setColor(0, 0, 0);
    for(int i=1; i<MENU_OPTIONS; i++)
        drawLine(MENU_W, H*(0.2 + i*0.05), W*0.6, H*(0.2 + i*0.05));
    drawText("BACK", MENU_W_OFFSET, H*0.2, H*0.05); //option 0
    //show version info
    using namespace versionInfo;
    int baseFontSize = settings::WINDOW_H/30; //standard font size
    int hStart = H*0.3; //where to start the next line of text
    drawText("Date Compiled: " + to_str(__DATE__) + " " + to_str(__TIME__), W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("Compiler: " + compilerName + " v" + compilerVersion, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("C++ Version: " + cppVersion, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("Operating System: " + operatingSystem, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("SDL2 version: " + sdl2CompileVersion, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("SDL2_image version: " + sdl2ImageCompileVersion, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("SDL2_ttf version: " + sdl2TTFCompileVersion, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("Detected Display DPI: " + displayDPI, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("Font: " + fontName, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("Game Version: " + gameVersion, W*0.1, hStart, baseFontSize);
    hStart += baseFontSize;
    drawText("Game Creator: Kevin Liu", W*0.1, hStart, baseFontSize);
}
void operateOptionsMenu()
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
                menu_select.back() += MENU_OPTIONS-1;
                menu_select.back() %= MENU_OPTIONS;
                break;
            case SDLK_DOWN:
                menu_select.back()++;
                menu_select.back() %= MENU_OPTIONS;
                break;
            case SDLK_RETURN:
                switch(menu_select.back())
                {
                case 0:
                    nextMenu(menuScreen::gameSettings);
                    break;
                case 1:
                    nextMenu(menuScreen::video);
                    break;
                case 2:
                    prevMenu();
                    break;
                }
                break;
            case SDLK_ESCAPE:
                prevMenu();
                break;
            }
            break;
        }
    }
    int H = settings::WINDOW_H, W = settings::WINDOW_W;
    renderClear(100, 100, 100);
    drawText("OPTIONS", MENU_W_OFFSET, H*0.1, H*0.1);
    fillRect(MENU_W_OFFSET - W*0.01, H*0.19, MENU_W + W*0.02, H*(0.02 + 0.05*MENU_OPTIONS), 150, 150, 150); //outer box
    fillRect(MENU_W_OFFSET, H*0.2, MENU_W, H*0.05*MENU_OPTIONS, 200, 200, 200); //inner box
    fillRect(MENU_W_OFFSET, H*(0.2 + 0.05*menu_select.back()), MENU_W, H*0.05 + 1, 200, 150, 100); //highlight the selected option
    setColor(0, 0, 0);
    for(int i=1; i<MENU_OPTIONS; i++)
        drawLine(MENU_W_OFFSET, H*(0.2 + i*0.05), MENU_W + MENU_W_OFFSET, H*(0.2 + i*0.05));
    drawText("GAME", MENU_W_OFFSET, H*0.2, H*0.05); //option 0
    drawText("VIDEO", MENU_W_OFFSET, H*0.25, H*0.05); //option 1
    drawText("BACK", MENU_W_OFFSET, H*0.3, H*0.05); //option 2
}
void operateMenuScreen(menu_screen &x)
{
    using namespace settings;
    int &m = menu_select.back();
    int sz = x.options.size();
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
                m += sz;
                m %= sz+1;
                break;
            case SDLK_DOWN:
                m++;
                m %= sz+1;
                break;
            case SDLK_LEFT:
                if(m != sz)
                    x.dec(m);
                break;
            case SDLK_RIGHT:
                if(m != sz)
                    x.inc(m);
                break;
            case SDLK_RETURN:
                if(m != sz)
                    x.reset(m);
                else prevMenu();
                break;
            case SDLK_ESCAPE:
                prevMenu();
                break;
            }
            break;
        }
    }
    x.render();
}
void operateMenu()
{
    switch(cur_menu.back())
    {
    case menuScreen::mainMenu:
        operateMainMenu();
        break;
    case menuScreen::controls:
        operateControlMenu();
        break;
    case menuScreen::options:
        operateOptionsMenu();
        break;
    case menuScreen::versionInfo:
        operateVersionInfoMenu();
        break;
    case menuScreen::indivControl:
        operateIndivControlMenu();
        break;
    case menuScreen::video:
        operateMenuScreen(videoMenu);
        break;
    case menuScreen::gameSettings:
        operateMenuScreen(gameSettingsMenu);
        break;
    }
    updateScreen();
}
