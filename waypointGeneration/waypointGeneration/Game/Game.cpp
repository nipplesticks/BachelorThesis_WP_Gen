#include "waypointGenerationPCH.h"
#include "Game.h"
#include "../Rendering/Rendering/Renderer.h"
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
	m_water.Update();

	for (int i = 0; i < 4; i++)
		m_edges[i].Update();

	for (auto & b : m_buildings)
		b.Update();
}

void Game::Draw()
{
	m_terrain.Draw();
	for (int i = 0; i < 4; i++)
		m_edges[i].Draw();

	m_player.Draw();

	for (auto & b : m_buildings)
		b.Draw();

	m_water.Draw();
}

void Game::_playerFixYPosition()
{

}

void Game::_cameraControl(double dt)
{
	Window * wnd = Window::GetInstance();
	POINT mp = wnd->GetMousePosition();

	//wnd->MouseToCenter();
	/*float mouseDeltaX = (float)mp.x - (float)m_mouseReferencePosition.x;
	float mouseDeltaY = (float)mp.y - (float)m_mouseReferencePosition.y;

	DirectX::XMFLOAT2 camRotation(DirectX::XMConvertToRadians(mouseDeltaX) * MOUSE_SESITIVITY_X, DirectX::XMConvertToRadians(mouseDeltaY) * MOUSE_SESITIVITY_Y);

	m_camera.Rotate(camRotation.y, camRotation.x, 0.0f);*/

	DirectX::XMFLOAT3 translation(0, 0, 0);

	if (wnd->IsKeyPressed(Input::UP_ARROW) || mp.y < 50)
		translation.z += CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::DOWN_ARROW) || mp.y > wnd->GetWindowSize().y - 50)
		translation.z -= CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::RIGHT_ARROW) || mp.x > wnd->GetWindowSize().x - 75)
		translation.x += CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::LEFT_ARROW) || mp.x < 75)
		translation.x -= CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::SPACE))
		translation.y += CAMERA_XZ_SPEED * dt;
	if (wnd->IsKeyPressed(Input::C))
		translation.y -= CAMERA_XZ_SPEED * dt;

	if (!wnd->IsMousePressed(Input::MOUSE_CODE::MBUTTON))
		m_camera.Translate(translation);
	
	INT scrollDelta;
	if (scrollDelta = wnd->GetMouseWheelDelta(Input::VERTICAL))
	{
		DirectX::XMFLOAT2 mousePos, center;
		mousePos.x = mp.x;
		mousePos.y = mp.y;
		center.x = m_mouseReferencePosition.x;
		center.y = m_mouseReferencePosition.y;

		DirectX::XMFLOAT3 worldPos;
		if (Renderer::GetInstance()->GetMousePicking(worldPos))
		{
			DirectX::XMFLOAT4 camPos = m_camera.GetPosition();
			camPos.w = 0.0f;

			DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&worldPos), DirectX::XMLoadFloat4(&camPos)));

			direction = DirectX::XMVectorScale(direction, scrollDelta * CAMERA_ZOOM_SPEED);

			DirectX::XMFLOAT3 trans;
			DirectX::XMStoreFloat3(&trans, direction);
			
			camPos.x += trans.x;
			camPos.y += trans.y;
			camPos.z += trans.z;
			m_camera.SetPosition(camPos.x, camPos.y, camPos.z);
		}
		else
		{
			m_camera.Translate(0, 0, CAMERA_ZOOM_SPEED * scrollDelta, false);
		}
		
	}

	static bool PressedLastFrame = false;
	static bool SampleOffset = false;
	if (wnd->IsMousePressed(Input::MOUSE_CODE::MBUTTON)) // Rotate The camera;
	{
		static POINT _sMouseRef;
		static DirectX::XMFLOAT3 _sWorldPosRef;
		static DirectX::XMVECTOR _sCameraPosOffset;

		if (!PressedLastFrame)
		{
			_sMouseRef = mp;

			
			if (Renderer::GetInstance()->GetMousePicking(_sWorldPosRef))
			{
				PressedLastFrame = true;
				
			}
		}
		else
		{
			float mouseDeltaX = (float)mp.x - (float)_sMouseRef.x;
			float mouseDeltaY = (float)mp.y - (float)_sMouseRef.y;

			DirectX::XMFLOAT2 camRotation(DirectX::XMConvertToRadians(mouseDeltaX) * dt * MOUSE_SESITIVITY_X * 2.0f, DirectX::XMConvertToRadians(mouseDeltaY) * dt * MOUSE_SESITIVITY_Y * 2.0f);

			m_camera.Rotate(camRotation.y, camRotation.x, 0.0f);

			DirectX::XMFLOAT4 camPos = m_camera.GetPosition();
			camPos.w = 0.0f;

			float length = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&_sWorldPosRef), DirectX::XMLoadFloat4(&camPos))));
			DirectX::XMFLOAT4 dir = m_camera.GetDirectionVector();
			DirectX::XMVECTOR vDir = DirectX::XMLoadFloat4(&dir);
			DirectX::XMVECTOR vWPos = DirectX::XMLoadFloat3(&_sWorldPosRef);
			DirectX::XMVECTOR vNewCamPos = DirectX::XMVectorAdd(vWPos, DirectX::XMVectorScale(vDir, -length));

			if (!SampleOffset)
			{
				_sCameraPosOffset = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4(&camPos), vNewCamPos);
				SampleOffset = true;
			}

			//DirectX::XMStoreFloat4(&camPos, DirectX::XMVectorAdd(vNewCamPos, _sCameraPosOffset));
			DirectX::XMStoreFloat4(&camPos, vNewCamPos);
			m_camera.SetPosition(camPos.x, camPos.y, camPos.z);
		}

	}
	else
	{
		PressedLastFrame = false;
		SampleOffset = false;
	}

	if (wnd->IsKeyPressed(Input::BACKSPACE)) // Reset The Camera
	{
		m_camera.SetDirection(1, -2, 1);
	}

	if (wnd->IsMousePressed(Input::MOUSE_CODE::LBUTTON))
	{
		DirectX::XMFLOAT3 worldPos;
		if (Renderer::GetInstance()->GetMousePicking(worldPos))
		{
			m_player.SetPosition(worldPos.x, worldPos.y + 0.5f, worldPos.z);
		}
	}



	float distanceToCenter = 
		DirectX::XMVectorGetX(DirectX::XMVector3Length
		(DirectX::XMVectorSubtract(DirectX::XMVectorSet((float)TERRAIN_SIZE / 2, 0, (float)TERRAIN_SIZE / 2, 0),
			DirectX::XMLoadFloat4(&m_camera.GetPosition()))));

	float a2 = pow(((float)TERRAIN_SIZE - 1) / 2, 2);

	float c = sqrt(a2 + a2);
	float ma = max(c, m_maxHeight);
	float mi = max(c, m_minHeight);
	
	float farPlane = distanceToCenter + ma;
	float nearPlane = distanceToCenter - mi;

	m_camera.CreateProjectionMatrix(max(nearPlane, 0.01f), farPlane);
}

void Game::_loadTerrain()
{
	const int MIN = -15;
	const int MAX = 15;
	const float NOISE = 25.0f;
	
	m_maxHeight = MAX * NOISE;
	m_minHeight = abs(MIN * NOISE);

	m_terrainMesh = m_terrainCreator.CreateTerrainFromFloatList2(
		m_diamondSquare.CreateDiamondSquare(TERRAIN_SIZE, TERRAIN_SIZE, NOISE, MIN, MAX, 1),
		TERRAIN_SIZE,
		m_terrainTexture,
		m_terrainTex2D,
		m_edgeMeshes
	);


	for (int i = 0; i < 4; i++)
	{
		int size = m_edgeMeshes[i].size();
		int v = 0;
		int moreSize = 0;

		if (i == 0 || i == 3)
		{
			v = 1;
			moreSize = 1;
		}

		for (v; v < size + moreSize; v += 2)
		{
			int add = 0;
			if (moreSize == 1)
				add = -1;

			Vertex ver = m_edgeMeshes[i][v + add];
			ver.Position.y = MIN * NOISE;

			m_edgeMeshes[i].insert(m_edgeMeshes[i].begin() + v, ver);

			size++;
		}


		
		m_edges[i].SetVertices(&m_edgeMeshes[i]);
		m_edges[i].SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_edges[i].SetColor(0.1, 0.2f, 0.1f, 1.0f);
		m_edges[i].SetPickable(true);
	}

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

	static const DirectX::XMFLOAT4A _SXMPlane[] =
	{
		{-0.5, 0.0f, 0.5f, 1.0f}, {0.5, 0.0f, 0.5f, 1.0f}, {-0.5, 0.0f, -0.5f, 1.0f},
		{0.5, 0.0f, 0.5f, 1.0f}, {0.5, 0.0f, -0.5f, 1.0f}, {-0.5, 0.0f, -0.5f, 1.0f}
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


	/* CUBE */
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
	/* CUBE _ END*/

	/* PLANE */
	std::vector<Vertex> plane(6);
	Vertex v;
	v.Normal = DirectX::XMFLOAT4A(0, 1, 0, 0);
	v.UV = DirectX::XMFLOAT2A(0, 0);
	for (int i = 0; i < 6; i++)
	{
		v.Position = _SXMPlane[i];
		plane[i] = v;
	}

	m_XZPlane = plane;
	/* PLANE _ END*/

	m_player.SetVertices(&m_playerMesh);
	m_player.SetColor(1.0f, 0.0f, 0.0f);
	m_water.SetVertices(&m_XZPlane);
	m_water.SetColor(0.0f, 0.26015625f, 0.3265625f, 0.6f);
	//m_water.SetColor(0.0f, 0.26015625f, 0.3265625f, 1.0f);
	
	m_water.SetScale((float)TERRAIN_SIZE - 1.5f, 1.0f, (float)TERRAIN_SIZE - 1.5f);
	m_water.SetPosition((float)(TERRAIN_SIZE - 1) * 0.5f, m_terrainCreator.WATER_START, (float)(TERRAIN_SIZE - 1) * 0.5f);


	m_terrain.SetPickable(true);
	
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
