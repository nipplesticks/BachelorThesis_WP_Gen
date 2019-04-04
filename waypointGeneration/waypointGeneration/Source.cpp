#include "waypointGenerationPCH.h"
#include "Rendering/Rendering/Renderer.h"
#include "Rendering/Camera/Camera.h"
#include "MapGeneration/DiamondSquare.h"
#include "MapGeneration/TerrainCreator.h"

#define MOUSE_SESITIVITY_X	0.05
#define MOUSE_SESITIVITY_Y	0.05
#define CAMERA_XZ_SPEED		10.0
#define CAMERA_Y_SPEED		10.0



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
	camera.CreateProjectionMatrix(0.01f, 10000.0f);
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

	// Heightmap test
	int mapSize = 65;
	DiamondSquare diamondSquare;
	std::vector<float> heightmap = diamondSquare.CreateDiamondSquare(mapSize, 10, 10.0f);

	const int arraySize = 4;
	int mapSizes[arraySize];
	int stepSizes[arraySize];
	float noises[arraySize];

	mapSizes[0] = 100;
	mapSizes[1] = 50;
	mapSizes[2] = 25;
	mapSizes[3] = 125;

	stepSizes[0] = 10;
	stepSizes[1] = 10;
	stepSizes[2] = 5;
	stepSizes[3] = 125;

	noises[0] = 2.5f;
	noises[1] = 7.5f;
	noises[2] = 0.5f;
	noises[3] = 10.0f;

	DiamondSquare advancedMap;
	std::vector<std::vector<float>> advancedHeightmap = advancedMap.AdvancedCreateDiamondSquare(mapSizes, stepSizes, noises, arraySize);

	TerrainCreator terrainCreator;
	std::vector<Vertex> vertices = terrainCreator.CreateTerrainFromFloatList(heightmap, mapSize);
	std::vector<Vertex> temp;
	for (int i = 0; i < arraySize; i++)
	{
		temp = terrainCreator.CreateTerrainFromFloatList(advancedHeightmap[i], mapSizes[i]);
	}

	Drawable map;
	map.SetColor(0.3f, 1.0f, 0.02f);
	map.SetVertices(&vertices);
	map.SetPosition(-50, 0, -50);


	while (wnd->IsOpen())
	{
		POINT mousePos = wnd->GetMousePosition();
		wnd->MouseToCenter();
		double dt = deltaTime.Stop();

		if (wnd->IsKeyPressed(Input::ESCAPE))
			wnd->Close();

		DirectX::XMFLOAT3 translation(0, 0, 0);

		if (wnd->IsKeyPressed(Input::W))
			translation.z += CAMERA_XZ_SPEED * dt;
		if (wnd->IsKeyPressed(Input::S))
			translation.z -= CAMERA_XZ_SPEED * dt;
		if (wnd->IsKeyPressed(Input::D))
			translation.x += CAMERA_XZ_SPEED * dt;
		if (wnd->IsKeyPressed(Input::A))
			translation.x -= CAMERA_XZ_SPEED * dt;
		if (wnd->IsKeyPressed(Input::SPACE))
			translation.y += CAMERA_Y_SPEED * dt;
		if (wnd->IsKeyPressed(Input::C))
			translation.y -= CAMERA_Y_SPEED * dt;

		float mouseDeltaX = (float)mousePos.x - (float)mouseReference.x;
		float mouseDeltaY = (float)mousePos.y - (float)mouseReference.y;

		DirectX::XMFLOAT2 camRotation(DirectX::XMConvertToRadians(mouseDeltaX) * MOUSE_SESITIVITY_X, DirectX::XMConvertToRadians(mouseDeltaY) * MOUSE_SESITIVITY_Y);

		camera.Rotate(camRotation.y, camRotation.x, 0.0f);
		camera.Translate(translation, false);
		camera.Update();

		test.Rotate(0, 1.0f * dt, 0);
		test.Update();

		map.Update();

		renderer->Clear();

		test.Draw();
		map.Draw();

		renderer->Flush();
		renderer->Present();
	}

	renderer->Release();
	
	return 0;
}
