#include "waypointGenerationPCH.h"
#include "Rendering/Rendering/Renderer.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1280, 720, 0);

	Renderer * renderer = Renderer::GetInstance();
	renderer->Init();

	while (wnd->IsOpen())
	{
		if (wnd->IsKeyPressed(Input::ESCAPE))
			wnd->Close();






	}

	renderer->Release();

	return 0;
}
