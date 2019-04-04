#include "waypointGenerationPCH.h"
#include "Rendering/Rendering/Renderer.h"
#include "Game/Game.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1280, 720, 0);

	Renderer * renderer = Renderer::GetInstance();
	renderer->Init();
	
	Timer deltaTime;
	deltaTime.Start();

	Game * game = new Game();

	while (wnd->IsOpen())
	{
		double dt = deltaTime.Stop();

		if (wnd->IsKeyPressed(Input::ESCAPE))
			wnd->Close();

		game->Update(dt);

		renderer->Clear();

		game->Draw();

		renderer->Flush();
		renderer->Present();
	}

	renderer->Release();
	
	return 0;
}
