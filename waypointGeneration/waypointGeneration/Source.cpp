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
	_allocConsole();
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1280, 720, 0, "Kandidatarbete", "Frametime: 0 ms Frame: 0");

	HWND hwnd = wnd->GetHwnd();
	SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, wnd->GetWindowSize().x + 400, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	RECT r;
	GetWindowRect(consoleWindow, &r);
	MoveWindow(consoleWindow, r.left, r.top, 1920 - wnd->GetWindowSize().x, 1080, TRUE);

	Renderer * renderer = Renderer::GetInstance();
	renderer->Init();
	
	Game * game = new Game();
	Timer deltaTime;
	deltaTime.Start();

	double totalTime = 0.0;
	UINT frameCounter = 0;

	bool first = true;

	while (wnd->IsOpen())
	{
		if (wnd->IsKeyPressed(Input::R) || game->GameOver())
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
		totalTime += dt;

		if (totalTime > 1.0 && frameCounter > 0)
		{
			double ms = (totalTime / frameCounter) * 1000;
			wnd->SetWindowTitle("Frametime: " + std::to_string(ms) + " ms" + " Frames: " + std::to_string(frameCounter));

			totalTime -= 1.0;
			frameCounter = 0;
		}

		frameCounter++;
	}
	delete game;

	renderer->Release();
	
	return 0;
}
