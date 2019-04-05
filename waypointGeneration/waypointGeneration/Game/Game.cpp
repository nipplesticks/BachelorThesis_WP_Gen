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
	m_mouseReferencePosition = wnd->GetMousePosition();

	_setupGame();
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
	POINT mp = wnd->GetMousePosition();
	
	DirectX::XMFLOAT3 translation(0, 0, 0);

	if (wnd->IsKeyPressed(Input::UP_ARROW) || mp.y < 50)
		translation.z += CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::DOWN_ARROW) || mp.y > wnd->GetWindowSize().y - 50)
		translation.z -= CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::RIGHT_ARROW) || mp.x > wnd->GetWindowSize().x - 75)
		translation.x += CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::LEFT_ARROW) || mp.x < 75)
		translation.x -= CAMERA_XZ_SPEED * dt;
	
	INT scrollDelta;
	if (scrollDelta = wnd->GetMouseWheelDelta(Input::VERTICAL))
	{
		DirectX::XMFLOAT2 mousePos, center;
		mousePos.x = mp.x;
		mousePos.y = mp.y;
		center.x = m_mouseReferencePosition.x;
		center.y = m_mouseReferencePosition.y;

		DirectX::XMFLOAT2 zoomDir;
		DirectX::XMStoreFloat2(&zoomDir, DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&mousePos), DirectX::XMLoadFloat2(&center)));

		float x = zoomDir.x * dt * scrollDelta * 9;
		float y = zoomDir.y * dt * scrollDelta * -16;
		float z = CAMERA_ZOOM_SPEED * dt * scrollDelta;

		m_camera.Translate(0, 0, CAMERA_ZOOM_SPEED * dt * scrollDelta, false);
		m_camera.Translate(x, 0, y);
	}

	if (wnd->IsKeyPressed(Input::SPACE))
		translation.y += CAMERA_ZOOM_SPEED * dt;
	if (wnd->IsKeyPressed(Input::C))
		translation.y -= CAMERA_ZOOM_SPEED * dt;

	/*float mouseDeltaX = (float)mousePos.x - (float)m_mousePosLastFrame.x;
	float mouseDeltaY = (float)mousePos.y - (float)m_mousePosLastFrame.y;*/

	//DirectX::XMFLOAT2 camRotation(DirectX::XMConvertToRadians(mouseDeltaX) * MOUSE_SESITIVITY_X, DirectX::XMConvertToRadians(mouseDeltaY) * MOUSE_SESITIVITY_Y);

	//m_camera.Rotate(camRotation.y, camRotation.x, 0.0f);

	m_camera.Translate(translation);
}

void Game::_loadTerrain()
{
	m_terrainMesh = m_terrainCreator.CreateTerrainFromFloatList(
		m_diamondSquare.CreateDiamondSquare(TERRAIN_SIZE, TERRAIN_SIZE, 25.0f, -15, 15, 1),

		TERRAIN_SIZE,
		m_terrainTexture,
		m_terrainTex2D
	);

	m_terrain.SetVertices(&m_terrainMesh);
	//m_terrain.SetPosition(-TERRAIN_SIZE / 2, 0.0f, -TERRAIN_SIZE / 2);
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

void Game::_setupGame()
{
	const auto & startPos = m_terrainMesh.front().Position;
	m_player.SetPosition(startPos.x, startPos.y + 0.5f, startPos.z);
	m_camera.SetDirection(1, -2, 1);

	const DirectX::XMFLOAT4 camDir = m_camera.GetDirectionVector();

	DirectX::XMVECTOR camPosDir = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&camDir), -1.0f);
	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&m_player.GetPosition());
	DirectX::XMVECTOR camPos = DirectX::XMVectorAdd(playerPos, DirectX::XMVectorScale(camPosDir, 100.0f));

	DirectX::XMFLOAT3 xmCamPos;
	DirectX::XMStoreFloat3(&xmCamPos, camPos);
	m_camera.SetPosition(xmCamPos);
}
