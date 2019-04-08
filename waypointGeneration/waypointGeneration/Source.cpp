#include "waypointGenerationPCH.h"
#include "Rendering/Rendering/Renderer.h"
#include "Game/Game.h"

void _alocConsole() {
	AllocConsole();
	FILE* fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
}
	

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	srand(time(0));
	_alocConsole();
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1920, 1080, 0);

	Renderer * renderer = Renderer::GetInstance();
	renderer->Init();
	
	Game * game = new Game();
	Timer deltaTime;
	deltaTime.Start();

	while (wnd->IsOpen())
	{
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

	renderer->Release();
	
	return 0;
}
