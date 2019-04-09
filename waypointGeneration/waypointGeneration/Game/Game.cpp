#include "waypointGenerationPCH.h"
#include "Game.h"
#include "../Rendering/Rendering/Renderer.h"
Game::Game()
{
	_createWaterTexture();
	_loadMeshes();
	_loadTerrain();
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
	m_waterTex2D->Release();
	m_waterTexture->Release();

	m_wp.clear();
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


	static DirectX::XMFLOAT2 waterUV = { 0,0 };
	static double counter = 0;

	counter += dt * 0.1;

	waterUV.x = counter;
	waterUV.y = waterUV.x;

	m_water.SetUVOffset(waterUV.x, waterUV.y);
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

	for (auto & d : m_wp)
		d.Draw();

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

#include "water_texture.h"

void Game::_createWaterTexture()
{
	DirectX::XMFLOAT4 waterArr[WATER_WIDTH * WATER_HEIGHT];

	const char * data = header_data;

	for (int i = 0; i < WATER_WIDTH * WATER_HEIGHT; i++)
	{
		float pixel[3];
		HEADER_PIXEL(data, pixel);

		waterArr[i].x = pixel[0] / 256.0f;
		waterArr[i].y = pixel[1] / 256.0f;
		waterArr[i].z = pixel[2] / 256.0f;
		waterArr[i].w = 1.0f;
	}

	D3D11_TEXTURE2D_DESC desc = {};
	desc.ArraySize = 1;
	desc.Height = WATER_HEIGHT;
	desc.Width = WATER_WIDTH;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA subData;
	subData.pSysMem = waterArr;
	subData.SysMemPitch = WATER_WIDTH * sizeof(DirectX::XMFLOAT4);

	ID3D11Device * device = Renderer::GetInstance()->GetDevice();

	HRESULT hr = device->CreateTexture2D(&desc, &subData, &m_waterTex2D);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(m_waterTex2D, &srvDesc, &m_waterTexture);
	int lol = 32;
}

void Game::_loadTerrain()
{
	Timer t;

	const int MIN = -10;
	const int MAX = 10;
	const float NOISE = 25.0f;
	
	bool placedPlayer = false;

	m_maxHeight = MAX * NOISE;
	m_minHeight = abs(MIN * NOISE);

	t.Start();
	m_terrainMesh = m_terrainCreator.CreateTerrainFromFloatList2(
		m_diamondSquare.CreateDiamondSquare(TERRAIN_SIZE, TERRAIN_SIZE - 1, NOISE, MIN, MAX, 4),
		TERRAIN_SIZE,
		m_terrainTexture,
		m_terrainTex2D,
		m_edgeMeshes
	);
	double time = t.Stop(Timer::MILLISECONDS);
	std::cout << "\nTime to generate terrain mesh: " << time << " ms\n";
	long int lastX = 0;
	long int lastY = 0;
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0, -1, 0, 0);
	std::cout << "\nGenerating waypoints\n";

	// TODO :: Remove Waypoints under water
	// TODO :: Push Blocked triangles to Terrain Quad Tree

	for (int i = 0; i < m_terrainMesh.size(); i+=3)
	{
		if (i % 1000 == 0)
			std::cout << "\r" << ((double)i / (double)m_terrainMesh.size() )* 100.0;

		DirectX::XMFLOAT4A v0, v1, v2;
		v0 = m_terrainMesh[i].Position;
		v1 = m_terrainMesh[i + 1].Position;
		v2 = m_terrainMesh[i + 2].Position;

		DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
		DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

		//DirectX::XMStoreFloat4A(&m_terrainMesh[i].Normal, normal);
		//DirectX::XMStoreFloat4A(&m_terrainMesh[i + 1].Normal, normal);
		//DirectX::XMStoreFloat4A(&m_terrainMesh[i + 2].Normal, normal);

		float dot = fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, up)));
		if (dot < m_terrainCreator.UNWALKABLE_SURFACE)
		{
			for (int k = i; k < i + 3; k++)
			{
				DirectX::XMFLOAT4A pos = m_terrainMesh[k].Position;
				


				Waypoint wp(pos.x, pos.z);
				wp.SetHeightVal(pos.y);

				long int xPos = pos.x;
				long int yPos = pos.z;

				if (xPos > lastX)
					lastX = xPos;

				if (yPos > lastY)
					lastY = yPos;

				long int key = xPos + yPos * (TERRAIN_SIZE);

				auto it = m_waypoints.find(key);

				if (it == m_waypoints.end())
				{
					m_waypoints.insert(std::make_pair(key, wp));

				}
			}
		}
		else if (!placedPlayer && i > TERRAIN_SIZE * TERRAIN_SIZE * 0.5f - 10)
		{
			DirectX::XMFLOAT3 playerPos;
			playerPos.x = m_terrainMesh[i].Position.x + m_terrainMesh[i + 1].Position.x + m_terrainMesh[i + 2].Position.x;
			playerPos.y = m_terrainMesh[i].Position.y + m_terrainMesh[i + 1].Position.y + m_terrainMesh[i + 2].Position.y;
			playerPos.z = m_terrainMesh[i].Position.z + m_terrainMesh[i + 1].Position.z + m_terrainMesh[i + 2].Position.z;

			playerPos.x /= 3.0f;
			playerPos.y /= 3.0f;
			playerPos.z /= 3.0f;
			playerPos.y += 0.5f;

			m_player.SetPosition(playerPos);
			placedPlayer = true;
		}
	}

	time = t.Stop(Timer::MILLISECONDS);
	std::cout << "\nTime to generate waypoints: " << time << " ms\n";
	std::cout << "\nNumber of waypoints:" << m_waypoints.size() << std::endl;

	std::map<long int, long int> ereasedVals;

	for (long int i = 0; i < lastY; i++)
	{
		for (long int j = 0; j < lastX; j++)
		{
			long int key = j + i * TERRAIN_SIZE;

			bool erase = true;

			for (int y = i - 1; y <= i + 1 && erase; y++)
			{
				for (int x = j - 1; x <= j + 1 && erase; x++)
				{
					if (y == i && x == j)
						continue;

					int tKey = x + y * TERRAIN_SIZE;

					auto tIt = m_waypoints.find(tKey);
					auto eIt = ereasedVals.find(tKey);
					erase = tIt != m_waypoints.end() || eIt != ereasedVals.end();
				}
			}
			if (erase)
			{
				m_waypoints.erase(key);
				ereasedVals.insert(std::make_pair(key, key));
			}
		}
	}
	ereasedVals.clear();

	for (long int i = 0; i < lastY; i++)
	{
		for (long int j = 0; j < lastX; j++)
		{
			long int key = j + i * TERRAIN_SIZE;

			bool erase = true;

			for (int y = i - 1; y <= i + 1 && erase; y++)
			{
				if (y == i)
					continue;
				int tKey = j + y * TERRAIN_SIZE;
				auto tIt = m_waypoints.find(tKey);
				auto eIt = ereasedVals.find(tKey);
				erase = tIt != m_waypoints.end() || eIt != ereasedVals.end();

			}
			if (!erase)
			{
				erase = true;
				for (int x = j - 1; x <= j + 1 && erase; x++)
				{
					if (x == j)
						continue;

					int tKey = x + i * TERRAIN_SIZE;

					auto tIt = m_waypoints.find(tKey);
					auto eIt = ereasedVals.find(tKey);
					erase = tIt != m_waypoints.end() || eIt != ereasedVals.end();
				}
			}
			if (erase)
			{
				m_waypoints.erase(key);
				ereasedVals.insert(std::make_pair(key, key));
			}
		}
	}


	time = t.Stop(Timer::MILLISECONDS);
	std::cout << "\nTime to cleanup the waypoints: " << time << " ms\n";
	std::cout << "\nNumber of waypoints after cleanup: " << m_waypoints.size() << std::endl;

	if (DRAW_WAYPOINT)
	{
		m_wp = std::vector<Drawable>(m_waypoints.size());
		int counter = 0;
		for (auto & w : m_waypoints)
		{
			m_wp[counter].SetPosition(w.second.GetPosition().x, w.second.GetHeightVal(), w.second.GetPosition().y);
			m_wp[counter].SetVertices(&m_playerMesh);
			m_wp[counter].SetScale(0.5f, 0.5f, 0.5f);
			m_wp[counter].Update();
			m_wp[counter++].SetColor(1, 0, 0);
		}

		time = t.Stop(Timer::MILLISECONDS);
		std::cout << "Time to create Drawables: " << time << " ms\n";
	}


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
	m_water.SetScale((float)TERRAIN_SIZE - 1.5f, 1.0f, (float)TERRAIN_SIZE - 1.5f);

	std::vector<Vertex> plane(6);
	Vertex v;
	v.Normal = DirectX::XMFLOAT4A(0, 1, 0, 0);
	v.UV = DirectX::XMFLOAT2A(0, 0);
	for (int i = 0; i < 6; i++)
	{
		v.Position = _SXMPlane[i];
		plane[i] = v;
	}

	auto scl = m_water.GetScale();
	scl.x /= 10.0f;
	scl.z /= 10.0f;

	plane[0].UV = DirectX::XMFLOAT2A(0, 0);
	plane[1].UV = DirectX::XMFLOAT2A(scl.x, 0);
	plane[2].UV = DirectX::XMFLOAT2A(0, scl.z);
	plane[3].UV = DirectX::XMFLOAT2A(scl.x, 0);
	plane[4].UV = DirectX::XMFLOAT2A(scl.x, scl.z);
	plane[5].UV = DirectX::XMFLOAT2A(0, scl.z);


	m_XZPlane = plane;
	/* PLANE _ END*/

	m_player.SetVertices(&m_playerMesh);
	m_player.SetColor(1.0f, 0.0f, 0.0f);
	m_water.SetVertices(&m_XZPlane);
	m_water.SetColor(1.0f, 1.0f, 1.0f, 0.6f);
	m_water.SetTexture(m_waterTexture);
	
	m_water.SetPosition((float)(TERRAIN_SIZE - 1) * 0.5f, m_terrainCreator.WATER_START, (float)(TERRAIN_SIZE - 1) * 0.5f);


	m_terrain.SetPickable(true);
	
}

void Game::_randomizeBuildings()
{
}

void Game::_setupGame()
{
	//const auto & startPos = m_terrainMesh.front().Position;
	//m_player.SetPosition(startPos.x, startPos.y + 0.5f, startPos.z);
	m_camera.SetDirection(1, -2, 1);

	const DirectX::XMFLOAT4 camDir = m_camera.GetDirectionVector();

	DirectX::XMVECTOR camPosDir = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&camDir), -1.0f);
	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&m_player.GetPosition());
	DirectX::XMVECTOR camPos = DirectX::XMVectorAdd(playerPos, DirectX::XMVectorScale(camPosDir, 100.0f));

	DirectX::XMFLOAT3 xmCamPos;
	DirectX::XMStoreFloat3(&xmCamPos, camPos);
	m_camera.SetPosition(xmCamPos);
}
