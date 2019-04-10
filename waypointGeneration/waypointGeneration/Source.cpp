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
	// HOW TO RAY vs AABB
	/*DirectX::XMVECTOR mMin = DirectX::XMVectorSet(-1, -1, 0, 0),
		mMax = DirectX::XMVectorSet(1, 1, 0, 0);
	
	DirectX::BoundingBox bb;
	bb.CreateFromPoints(bb, mMin, mMax);

	DirectX::XMVECTOR origin = DirectX::XMVectorSet(2, 0, 0,0);
	DirectX::XMVECTOR dir = DirectX::XMVector2Normalize(DirectX::XMVectorSet(-1, 1, 0, 0));

	float t;
	bool hit = bb.Intersects(origin, dir, t);

	if (hit)
	{
		DirectX::XMFLOAT3 iPoint;
		DirectX::XMVECTOR point = DirectX::XMVectorAdd(origin, DirectX::XMVectorScale(dir, t));
		DirectX::XMStoreFloat3(&iPoint, point);
	}*/

	// HOW TO RAY vs TRI
	DirectX::XMVECTOR v0 = DirectX::XMVectorSet(-1, -1, 0, 0),
		v1 = DirectX::XMVectorSet(1, 1, 0, 0),
		v2 = DirectX::XMVectorSet(2, -1, 0, 0);

	DirectX::XMVECTOR origin = DirectX::XMVectorSet(5, 0, 0, 0);
	DirectX::XMVECTOR originEnd = DirectX::XMVectorSet(-5, 0, 0, 0);
	DirectX::XMVECTOR dir = DirectX::XMVector3Normalize(DirectX::XMVectorSet(0, 0, 1, 0));

	float t;
	
	bool hit = DirectX::TriangleTests::Intersects(origin, dir, v0, v1, v2, t);
	auto vec = DirectX::XMVector2IntersectLine(v0, v2, origin, originEnd);

	if (hit)
	{
		DirectX::XMFLOAT3 iPoint;
		DirectX::XMVECTOR point = DirectX::XMVectorAdd(origin, DirectX::XMVectorScale(dir, t));
		DirectX::XMStoreFloat3(&iPoint, point);
		int breakMe = 0;
	}


	srand(time(0));
	_allocConsole();
	Window * wnd = Window::GetInstance();
	wnd->Create(hInstance, nCmdShow, 1280, 720, 0);

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
