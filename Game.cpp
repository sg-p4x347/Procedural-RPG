#include "Game.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

Game::Game(unsigned int worldWidthIn, unsigned int regionWidthIn, unsigned int loadWidthIn) {
	worldWidth = worldWidthIn;
	regionWidth = regionWidthIn;
	loadWidth = loadWidthIn;
}
void Game::createWorld(int seed,string name) {
	// create the relative path
	workingPath = "saves/" + name + "/";
	// create the save directory
	CreateDirectory(wstring(workingPath.begin(), workingPath.end()).c_str(), NULL);
	unsigned int regionSize = (regionWidth + 1)*(regionWidth + 1)*sizeof(short);
	// seed the RNG
	srand(seed);
	// generate the terrain distribution maps
	Distribution *continentMap = new Distribution(16, 0.4, 2, true);
	Distribution *biomeMap = new Distribution(64, 1.0, 2, false);
	Distribution *terrain = new Distribution(16, 0.4, 5, continentMap, biomeMap);

	//-------------------------------
	// save terrain to binary file
	//-------------------------------

	unsigned int bufferSize = regionSize*(terrain->getWidth() / regionWidth)*(terrain->getWidth() / regionWidth);
	unsigned char *terrainBuffer = new unsigned char[bufferSize];
	// iterate through each region
	unsigned int index = 0;
	for (unsigned short regionY = 0; regionY < (terrain->getWidth()/regionWidth); regionY++) {
		for (unsigned short regionX = 0; regionX < (terrain->getWidth() / regionWidth); regionX++) {
			// push each vertex in the region
			for (unsigned short vertY = 0; vertY <= regionWidth; vertY++) {
				for (unsigned short vertX = 0; vertX <= regionWidth; vertX++) {
					short vertex = short(terrain->points[regionX*regionWidth + vertX][regionY*regionWidth + vertY] * 10);
					terrainBuffer[index] = vertex & 0xff;
					terrainBuffer[index + 1] = (vertex >> 8) & 0xff;
					index += 2;
				}
			}
		}
	}
	//-----------------------------------------------
	// output file stream
	//-----------------------------------------------
	ofstream file(workingPath + "terrain.bin", ios::binary);
	file.seekp(0);
	if (file.is_open()) {
		// write the data
		file.write((const char *)terrainBuffer, bufferSize);
	}
	// close the stream
	file.close();
	delete[] terrainBuffer;
}
void Game::createPlayer(string name) {

}
void Game::loadWorld() {
	world.init(worldWidth,regionWidth,loadWidth,workingPath);
	world.loadPlayer();
	world.fillRegions(direct3D->GetDevice());
}
MSG Game::initGameLoop(MSG msg) {
	unsigned long lastTime = GetTickCount();
	// Main message loop
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			if (runGame) {
				unsigned long elapsedTime = GetTickCount() - lastTime;
				if (elapsedTime >= timeStep) {
					// update
					input->Frame();
					update(double(elapsedTime)/1000); // the number of seconds that have passed
					// render
					render();

					// update the elapsed time
					lastTime = GetTickCount();
				}
			}
		}
	}
	return msg;
}
//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT Game::InitWindow(HINSTANCE hInstance, int nCmdShow) {
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"TutorialWindowClass";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return E_FAIL;

	// Create window
	g_hInst = hInstance;
	RECT rc = { 0, 0, 1900, 1080 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	g_hWnd = CreateWindow(L"TutorialWindowClass", L"Procedural RPG",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
		nullptr);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}
//-------------------------------
// Initialize the Direct3D system
//-------------------------------
HRESULT Game::InitDirectX() {
	bool result = true;
	RECT rc;
	GetClientRect(g_hWnd, &rc);
	int screenWidth = rc.right - rc.left;
	int screenHeight = rc.bottom - rc.top;
	direct3D = new D3DClass;
	direct3D->Initialize(screenWidth, screenHeight, false, g_hWnd, false, 1000.0f, 0.1f);
	if (!result)
	{
		MessageBox(g_hWnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return E_FAIL;
	}
	// init input handling
	input = new InputClass;
	result = input->Initialize(g_hInst, g_hWnd, screenWidth, screenHeight);
	if (!result) {
		return E_FAIL;
	}
	return S_OK;
}
HRESULT Game::InitShaders() {
	shaderManager = new ShaderManager;
	return shaderManager->initialize(direct3D->GetDevice(), g_hWnd);
}
//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void Game::CleanupDevice()
{
	direct3D->Shutdown();
}
//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// Note that this tutorial does not handle resizing (WM_SIZE) requests,
		// so we created the window without the resize border.

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Game::update(double elapsed) {
	world.update(direct3D->GetDevice(),input,elapsed);
}
HRESULT Game::render() {
	world.render(direct3D,shaderManager);
	return S_OK;
}
Game::~Game() {
	delete direct3D;
	delete shaderManager;
}