#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "waypointGenerationPCH.h"
#include "Rendering/Rendering/Renderer.h"
#include "Game/Game.h"

void _allocConsole() {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
}
	

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	//srand(time(0));
	_allocConsole();
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1920, 1080, 1);

	Renderer * renderer = Renderer::GetInstance();
	renderer->Init();
	
	Game * game = new Game();
	Timer deltaTime;
	deltaTime.Start();

	bool first = true;

	while (wnd->IsOpen())
	{
		if (wnd->IsKeyPressed(Input::R))
		{
			delete game;
			game = new Game();
		}

		double dt = deltaTime.Stop();

		if (wnd->IsKeyPressed(Input::ESCAPE))
			wnd->Close();

		game->Update(dt);
		wnd->ResetInput();

		renderer->Clear();

		game->Draw();

		renderer->Flush();
		renderer->Present();
		
	}
	delete game;

	renderer->Release();
	
	return 0;
}
