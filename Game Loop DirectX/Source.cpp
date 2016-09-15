#include <windows.h>
#include <d3d9.h>
#include <time.h>
#include <iostream>
using namespace std;

#pragma comment(lib,"d3d9.lib")

//program settings
const string APPTITLE = "Direct3D_Windowed";
const int SCREENW = 640;
const int SCREENH = 480;

extern int width = 20;
extern int height = 20;
extern int posRight = 20;
extern int bottom = 20;
bool isBegin = false;
void Game_End(HWND hwnd);

// previous rect
extern int preRight = 20;
extern int preLeft = 20;
extern int preTop = 20;
extern int preBottom = 20;
//Direct3D objects
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface = NULL;
bool gameover = false;
void DrawCustomRect(int right, int bottom) {
	RECT rect;
	int r, g, b;
	r = rand() % 255;
	g = rand() % 255;
	b = rand() % 255;
	d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(r, g, b));
	//d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(0, 255, 255));
	rect.left = right - width;
	rect.right = right;
	rect.top = bottom - height;
	rect.bottom = bottom;
	// save for the previous position
	preLeft = rect.left;
	preRight = rect.right;
	preTop = rect.top;
	preBottom = rect.bottom;
	d3ddev->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE);
}
void ClearRect(int left, int right, int top, int bottom) {
	RECT rect;
	d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(255, 255, 255));
	//d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(0, 255, 255));
	rect.left = left;
	rect.right = right;
	rect.top = top;
	rect.bottom = bottom;
	d3ddev->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE);
}
// Windows event handling function
LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Game_End(hWnd);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
//macro to detect key presses
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)


// Game initialization function
bool Game_Init(HWND hwnd)
{
	HRESULT result;
	//initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		MessageBox(hwnd, "Error initializing Direct3D", "Error", MB_OK);
		return FALSE;
	}
	//set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // just clear it
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = SCREENW;
	d3dpp.BackBufferHeight = SCREENH;
	d3dpp.hDeviceWindow = hwnd;
	//create Direct3D device
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hwnd, // which window
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, // type of fuctinon in video card
		&d3dpp,
		&d3ddev); // point to pointer

	if (d3ddev == NULL)
	{
		MessageBox(hwnd, "Error creating Direct3D device", "Error", MB_OK);
		return FALSE;
	}
	// set random speed
	srand(time(NULL));
	// Clear the backbuffer
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	// create surface
	result = d3ddev->CreateOffscreenPlainSurface(
		100,
		100,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&surface,
		NULL);
	if (!result) {
		return 1;
	}
	return 1;
}

// Game update function
void Game_Run(HWND hwnd)
{
	//make sure the Direct3D device is valid
	if (!d3ddev) return;

	//clear the backbuffer to bright green
	//d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 255), 1.0f, 0);
	
	//start rendering
	if (d3ddev->BeginScene())
	{
		// configure rect's position
		if (posRight - width < SCREENW && bottom == height) {
			posRight += width;
			bottom = height;
			// draw at corner
			if (posRight == SCREENW) {
				DrawCustomRect(posRight, bottom);
			}
		}
		if (posRight == SCREENW && bottom <= SCREENH) {
			bottom += height;
			posRight = SCREENW;
			// draw at corner
			if (bottom == SCREENH) {
				DrawCustomRect(posRight, bottom);
			}
		}
		if (posRight - width >= 0 && bottom == SCREENH) {
			posRight -= width;
			bottom == SCREENH;
			// draw at corner
			if (posRight == width) {
				DrawCustomRect(posRight, bottom);
			}
		}
		if (posRight - width == 0 && bottom - height > 0) {
			bottom -= height;
			posRight = width;
		}
		DrawCustomRect(posRight, bottom);
		//ClearRect(preLeft, preRight, preTop, preBottom);
		//stop rendering
		d3ddev->EndScene();

		//copy back buffer to the frame buffer
		
	}
	d3ddev->Present(NULL, NULL, NULL, NULL);
	//check for escape key (to exit program)
	if (KEY_DOWN(VK_ESCAPE))
		PostMessage(hwnd, WM_DESTROY, 0, 0);

}

// Game shutdown function
void Game_End(HWND hwnd){
	surface->Release();
	if (d3ddev)
	{
		d3ddev->Release();
		d3ddev = NULL;
	}
	if (d3d)
	{
		d3d->Release();
		d3d = NULL;
	}
}




// Main Windows entry function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	MSG msg;

	//set the new window's properties
	//previously found in the MyRegisterClass function
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.lpszMenuName = NULL;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "MainWindowClass";

	if (!RegisterClassEx(&wc))
		return FALSE;

	//create a new window
	//previously found in the InitInstance function
	HWND hwnd = CreateWindow("MainWindowClass", APPTITLE.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT - 50,
		SCREENW, SCREENH, (HWND)NULL,
		(HMENU)NULL, hInstance, (LPVOID)NULL);

	//was there an error creating the window?
	if (hwnd == 0) return 0;

	//display the window
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	//initialize the game
	if (!Game_Init(hwnd)) return FALSE;

	// main message loop
	while (!gameover)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Game_Run(hwnd);
		
		
		
	}
	// just test git fork
	Game_End(hwnd);

	return msg.wParam;
}


