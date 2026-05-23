#include "Game.h"

// Windows entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    Game* pGame = new Game("SRC Engine Game", 1280, 720);
    
    if(pGame->Initialize()) {
        pGame->Run();
    }
    
    pGame->Shutdown();
    delete pGame;
    
    return 0;
}