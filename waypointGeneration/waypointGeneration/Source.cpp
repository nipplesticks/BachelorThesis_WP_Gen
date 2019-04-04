#include "waypointGenerationPCH.h"
#include "Rendering/Rendering/Renderer.h"
#include "Rendering/Camera/Camera.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1280, 720, 0);

	Renderer * renderer = Renderer::GetInstance();
	renderer->Init();
	
	Camera * camera = new Camera();
	camera->SetAsActiveCamera();
	camera->SetDirection(0.f, 0.f, 1.f);
	camera->CreateProjectionMatrix();
	camera->SetPosition(0.f, 0.f, -1.f);
	camera->Update();

	std::vector<Vertex> verts;
	Vertex vertToPush;
	vertToPush.Position = DirectX::XMFLOAT4A(0.0f, 0.5f, 0.0f, 1.0f);
	vertToPush.Normal = DirectX::XMFLOAT4A(0.0f, 0.0f, -1.0f, 0.0f);
	verts.push_back(vertToPush);
	vertToPush.Position = DirectX::XMFLOAT4A(0.5f, 0.5f, 0.0f, 1.0f);
	verts.push_back(vertToPush);
	vertToPush.Position = DirectX::XMFLOAT4A(0.0f, 0.0f, 0.0f, 1.0f);
	verts.push_back(vertToPush);

	Drawable test;
	test.SetVertices(&verts);
	test.SetColor(0.5f, 0.0f, 0.5f);
	
	while (wnd->IsOpen())
	{
		if (wnd->IsKeyPressed(Input::ESCAPE))
			wnd->Close();

		renderer->Clear();
		test.Draw();
		renderer->Flush();
		renderer->Present();
	}

	renderer->Release();
	delete camera;

	return 0;
}
