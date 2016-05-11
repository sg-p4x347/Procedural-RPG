#include "pch.h"
#include "Game.h"
#include "Utility.h"

using namespace DirectX;
using namespace Utility;

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow ) {
	//----------------
	// create the game
	//----------------
	static Game game(1024,32,4);
	game.createWorld(66,"testWorld");
	game.createPlayer("sg_p4x347");

    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );
	// create the window
    if(FAILED(game.InitWindow(hInstance, nCmdShow))) {
		return 0;
	}
	// initialize directX components
    if(FAILED(game.InitDirectX())) {
        game.CleanupDevice();
        return 0;
    }
	// initialize shaders
	if (FAILED(game.InitShaders())) {
		return 0;
	}
	// initialize the world
	game.loadWorld();
    // Main game loop
	MSG msg = { 0 };
	msg = game.initGameLoop(msg);

	// clean up everything, and close the window
    game.CleanupDevice();

    return ( int )msg.wParam;
}
