#include "waypointGenerationPCH.h"
#include "Game.h"

Game::Game()
{
	_loadTerrain();
	_loadMeshes();
	_randomizeBuildings();

	m_camera.CreateProjectionMatrix(0.01f, 10000);
	m_camera.SetAsActiveCamera();

	Window * wnd = Window::GetInstance();
	wnd->MouseToCenter();
	m_mousePosLastFrame = wnd->GetMousePosition();
}

Game::~Game()
{
	m_terrainTex2D->Release();
	m_terrainTexture->Release();
}

void Game::Update(double dt)
{
	/*
	
		PathFinding
	
	*/


	_playerFixYPosition();
	_cameraControl(dt);

	m_camera.Update();
	m_player.Update();
	m_terrain.Update();

	for (auto & b : m_buildings)
		b.Update();
}

void Game::Draw()
{
	m_terrain.Draw();
	m_player.Draw();

	for (auto & b : m_buildings)
		b.Draw();
}

void Game::_playerFixYPosition()
{

}

void Game::_cameraControl(double dt)
{
	Window * wnd = Window::GetInstance();
	POINT mousePos = wnd->GetMousePosition();
	wnd->MouseToCenter();

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

	float mouseDeltaX = (float)mousePos.x - (float)m_mousePosLastFrame.x;
	float mouseDeltaY = (float)mousePos.y - (float)m_mousePosLastFrame.y;

	DirectX::XMFLOAT2 camRotation(DirectX::XMConvertToRadians(mouseDeltaX) * MOUSE_SESITIVITY_X, DirectX::XMConvertToRadians(mouseDeltaY) * MOUSE_SESITIVITY_Y);

	m_camera.Rotate(camRotation.y, camRotation.x, 0.0f);
	m_camera.Translate(translation);
}

void Game::_loadTerrain()
{
	const int TERRAIN_SIZE = 1000;

	m_terrainMesh = m_terrainCreator.CreateTerrainFromFloatList(
		m_diamondSquare.CreateDiamondSquare(TERRAIN_SIZE, 5000, 1000),
		TERRAIN_SIZE,
		m_terrainTexture,
		m_terrainTex2D
	);

	m_terrain.SetVertices(&m_terrainMesh);
	m_terrain.SetPosition(-TERRAIN_SIZE / 2, 0.0f, -TERRAIN_SIZE / 2);
	m_terrain.SetTexture(m_terrainTexture);
}

void Game::_loadMeshes()
{
	static const DirectX::XMFLOAT4 _SXMcube[] =
	{
		{ 0.5,	-0.5,  0.5, 1.0f},	{-0.5,	-0.5,	-0.5, 1.0f},	{ 0.5,	-0.5,	-0.5, 1.0f},
		{-0.5,	 0.5, -0.5, 1.0f},	{ 0.5,	 0.5,	 0.5, 1.0f},	{ 0.5,	 0.5,	-0.5, 1.0f},
		{ 0.5,	 0.5, -0.5, 1.0f},	{ 0.5,	-0.5,	 0.5, 1.0f},	{ 0.5,	-0.5,	-0.5, 1.0f},
		{ 0.5,	 0.5,  0.5, 1.0f},	{-0.5,	-0.5,	 0.5, 1.0f},	{ 0.5,	-0.5,	 0.5, 1.0f},
		{-0.5,	-0.5,  0.5, 1.0f},	{-0.5,	 0.5,	-0.5, 1.0f},	{-0.5,	-0.5,	-0.5, 1.0f},
		{ 0.5,	-0.5, -0.5, 1.0f},	{-0.5,	 0.5,	-0.5, 1.0f},	{ 0.5,	 0.5,	-0.5, 1.0f},
		{ 0.5,	-0.5,  0.5, 1.0f},	{-0.5,	-0.5,	 0.5, 1.0f},	{-0.5,	-0.5,	-0.5, 1.0f},
		{-0.5,	 0.5, -0.5, 1.0f},	{-0.5,	 0.5,	 0.5, 1.0f},	{ 0.5,	 0.5,	 0.5, 1.0f},
		{ 0.5,	 0.5, -0.5, 1.0f},	{ 0.5,	 0.5,	 0.5, 1.0f},	{ 0.5,	-0.5,	 0.5, 1.0f},
		{ 0.5,	 0.5,  0.5, 1.0f},	{-0.5,	 0.5,	 0.5, 1.0f},	{-0.5,	-0.5,	 0.5, 1.0f},
		{-0.5,	-0.5,  0.5, 1.0f},	{-0.5,	 0.5,	 0.5, 1.0f},	{-0.5,	 0.5,	-0.5, 1.0f},
		{ 0.5,	-0.5, -0.5, 1.0f},	{-0.5,	-0.5,	-0.5, 1.0f},	{-0.5,	 0.5,	-0.5, 1.0f}
	};

	static const DirectX::XMFLOAT2A _SXMUV[] = 
	{
		{0.0f, 0.0f},
		{0.0f, 1.0f},
		{1.0f, 1.0f},

		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
	};

	std::vector<Vertex> verts;
	for (int i = 0; i < _countof(_SXMcube); i++)
	{
		Vertex v;
		v.Position.x = _SXMcube[i].x;
		v.Position.y = _SXMcube[i].y;
		v.Position.z = _SXMcube[i].z;
		v.Position.w = _SXMcube[i].w;

		v.UV = _SXMUV[i % 6];

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
	m_playerMesh = verts;
	
	m_player.SetVertices(&m_playerMesh);
	m_player.SetColor(1.0f, 0.0f, 0.0f);
}

void Game::_randomizeBuildings()
{
}
