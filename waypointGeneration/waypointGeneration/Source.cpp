#include "waypointGenerationPCH.h"
#include "Rendering/Rendering/Renderer.h"
#include "Rendering/Camera/Camera.h"

#define MOUSE_SESITIVITY_X 0.05
#define MOUSE_SESITIVITY_Y 0.05


static const DirectX::XMFLOAT4 _SXMcube[] =
{
	{ 0.5,	-0.5,  0.5, 0.5},	{-0.5,	-0.5,	-0.5, 0.5},	{ 0.5,	-0.5,	-0.5, 0.5},
	{-0.5,	 0.5, -0.5, 0.5},	{ 0.5,	 0.5,	 0.5, 0.5},	{ 0.5,	 0.5,	-0.5, 0.5},
	{ 0.5,	 0.5, -0.5, 0.5},	{ 0.5,	-0.5,	 0.5, 0.5},	{ 0.5,	-0.5,	-0.5, 0.5},
	{ 0.5,	 0.5,  0.5, 0.5},	{-0.5,	-0.5,	 0.5, 0.5},	{ 0.5,	-0.5,	 0.5, 0.5},
	{-0.5,	-0.5,  0.5, 0.5},	{-0.5,	 0.5,	-0.5, 0.5},	{-0.5,	-0.5,	-0.5, 0.5},
	{ 0.5,	-0.5, -0.5, 0.5},	{-0.5,	 0.5,	-0.5, 0.5},	{ 0.5,	 0.5,	-0.5, 0.5},
	{ 0.5,	-0.5,  0.5, 0.5},	{-0.5,	-0.5,	 0.5, 0.5},	{-0.5,	-0.5,	-0.5, 0.5},
	{-0.5,	 0.5, -0.5, 0.5},	{-0.5,	 0.5,	 0.5, 0.5},	{ 0.5,	 0.5,	 0.5, 0.5},
	{ 0.5,	 0.5, -0.5, 0.5},	{ 0.5,	 0.5,	 0.5, 0.5},	{ 0.5,	-0.5,	 0.5, 0.5},
	{ 0.5,	 0.5,  0.5, 0.5},	{-0.5,	 0.5,	 0.5, 0.5},	{-0.5,	-0.5,	 0.5, 0.5},
	{-0.5,	-0.5,  0.5, 0.5},	{-0.5,	 0.5,	 0.5, 0.5},	{-0.5,	 0.5,	-0.5, 0.5},
	{ 0.5,	-0.5, -0.5, 0.5},	{-0.5,	-0.5,	-0.5, 0.5},	{-0.5,	 0.5,	-0.5, 0.5}
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1280, 720, 0);

	Renderer * renderer = Renderer::GetInstance();
	renderer->Init();
	
	Camera camera;
	camera.SetAsActiveCamera();
	camera.SetDirection(0.f, 0.f, 1.f);
	camera.CreateProjectionMatrix();
	camera.SetPosition(0.f, 0.f, -10.f);


	std::vector<Vertex> verts;
	for (int i = 0; i < _countof(_SXMcube); i++)
	{
		Vertex v;
		v.Position.x = _SXMcube[i].x;
		v.Position.y = _SXMcube[i].y;
		v.Position.z = _SXMcube[i].z;
		v.Position.w = _SXMcube[i].w;

		verts.push_back(v);
	}

	for (size_t i = 0; i < verts.size(); i += 3)
	{
		Vertex v0 = verts[i];
		Vertex v1 = verts[i + 1];
		Vertex v2 = verts[i + 2];

		DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1.Position), DirectX::XMLoadFloat4A(&v0.Position));
		DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2.Position), DirectX::XMLoadFloat4A(&v0.Position));
		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));
		
		for (int k = i; k < i + 3; k++)
			DirectX::XMStoreFloat4A(&verts[k].Normal, normal);

	}

	Drawable test;
	test.SetVertices(&verts);
	test.SetColor(0.5f, 0.0f, 0.5f);

	Timer deltaTime;
	deltaTime.Start();

	wnd->MouseToCenter();
	POINT mouseReference = wnd->GetMousePosition();
	wnd->MouseToCenter();
	while (wnd->IsOpen())
	{
		POINT mousePos = wnd->GetMousePosition();
		wnd->MouseToCenter();
		double dt = deltaTime.Stop();

		if (wnd->IsKeyPressed(Input::ESCAPE))
			wnd->Close();

		DirectX::XMFLOAT3 translation(0, 0, 0);

		if (wnd->IsKeyPressed(Input::W))
			translation.z += 1.0f * dt;
		if (wnd->IsKeyPressed(Input::S))
			translation.z -= 1.0f * dt;
		if (wnd->IsKeyPressed(Input::D))
			translation.x += 1.0f * dt;
		if (wnd->IsKeyPressed(Input::A))
			translation.x -= 1.0f * dt;
		if (wnd->IsKeyPressed(Input::SPACE))
			translation.y += 1.0f * dt;
		if (wnd->IsKeyPressed(Input::C))
			translation.y -= 1.0f * dt;

		float mouseDeltaX = (float)mousePos.x - (float)mouseReference.x;
		float mouseDeltaY = (float)mousePos.y - (float)mouseReference.y;

		DirectX::XMFLOAT2 camRotation(DirectX::XMConvertToRadians(mouseDeltaX) * MOUSE_SESITIVITY_X, DirectX::XMConvertToRadians(mouseDeltaY) * MOUSE_SESITIVITY_Y);

		camera.Rotate(camRotation.y, camRotation.x, 0.0f);
		camera.Translate(translation, false);
		camera.Update();

		test.Rotate(0, 1.0f * dt, 0);
		test.Update();

		renderer->Clear();

		test.Draw();

		renderer->Flush();
		renderer->Present();
	}

	renderer->Release();
	
	return 0;
}
