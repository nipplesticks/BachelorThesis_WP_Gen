#include "waypointGenerationPCH.h"
#include "Game.h"
#include "../Rendering/Rendering/Renderer.h"
#include <time.h>
#include <algorithm>

Game::Game()
{
	if (USE_RANDOM_SEED)
	{
		srand(static_cast<unsigned>(time(0)));
	}


	_createWaterTexture();
	_loadMeshes();
	_createWorld();
	_randomizeBuildings();

	m_camera.CreateProjectionMatrix(5000.0f, 0.1f);
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

	for (auto & t : m_blockedTriangles)
		delete t;
	for (auto & t : m_unblockedTriangles)
		delete t;

	m_blockedTriangles.clear();
	m_unblockedTriangles.clear();
	
	m_wp.clear();
}

void Game::Update(double dt)
{
	/*
	
		PathFinding
	
	*/

	_playerFixYPosition(dt);
	_cameraControl(dt);

	m_camera.Update();
	m_player.Update();
	m_terrain.Update();


	static DirectX::XMFLOAT2 waterUV = { 0,0 };
	static double counter = 0;

	counter += dt * 0.1;

	waterUV.x = (float)counter;
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
	if (DRAW_CONNECTIONS && !m_connectionMesh.empty())
		m_connections.Draw();

	if (DRAW_TRIANGLES)
	{
		m_blockedtrianglesDraw.Draw();
		m_unblockedtrianglesDraw.Draw();
	}
	
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

void Game::_playerFixYPosition(double dt)
{
	Window * wnd = Window::GetInstance();
	float speed = 10;
	auto forward = m_camera.GetForwardVector();
	auto right = m_camera.GetRightVector();

	DirectX::XMFLOAT3 translate = { 0.0f, 0.0f, 0.0f };

	if (wnd->IsKeyPressed(Input::W))
	{
		translate.x += forward.x * speed * (float)dt;
		translate.y += forward.y * speed * (float)dt;
		translate.z += forward.z * speed * (float)dt;
	}
	if (wnd->IsKeyPressed(Input::S))
	{
		translate.x -= forward.x * speed * (float)dt;
		translate.y -= forward.y * speed * (float)dt;
		translate.z -= forward.z * speed * (float)dt;
	}
	if (wnd->IsKeyPressed(Input::A))
	{
		translate.x -= right.x * speed * (float)dt;
		translate.y -= right.y * speed * (float)dt;
		translate.z -= right.z * speed * (float)dt;
	}
	if (wnd->IsKeyPressed(Input::D))
	{
		translate.x += right.x * speed * (float)dt;
		translate.y += right.y * speed * (float)dt;
		translate.z += right.z * speed * (float)dt;
	}

	m_player.Translate(translate);

	DirectX::XMFLOAT3 iPoint;
	Triangle * tri = m_unblockedTriangleTree.RayIntersectionTriangle3D(m_player.GetPosition(), DirectX::XMFLOAT3(0, -1, 0), false, iPoint);

	if (tri == nullptr)
	{
		tri = m_blockedTriangleTree.RayIntersectionTriangle3D(m_player.GetPosition(), DirectX::XMFLOAT3(0, -1, 0), false, iPoint);
	}
	if (tri == nullptr)
	{
		tri = m_unblockedTriangleTree.RayIntersectionTriangle3D(m_player.GetPosition(), DirectX::XMFLOAT3(0, 1, 0), true, iPoint);
	}
	if (tri == nullptr)
	{
		tri = m_blockedTriangleTree.RayIntersectionTriangle3D(m_player.GetPosition(), DirectX::XMFLOAT3(0, 1, 0), true, iPoint);
	}

	if (tri)
	{
		/*
		DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&tri->normal);
		DirectX::XMVECTOR forward, right;
		float angle = 0.0f;
		
		angle = DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, DirectX::XMVectorSet(0, 1, 0, 0)));
		if (tri->normal.y < 0.99f)
		{
			right = (DirectX::XMVector3Cross(normal, DirectX::XMVectorSet(0, 1, 0, 0)));
			right = DirectX::XMVectorSetY(right, 0.0f);
			right = DirectX::XMVector3Normalize(right);
		}
		else
		{
			right = (DirectX::XMVector3Cross(normal, DirectX::XMVectorSet(0, 0, 1, 0)));
			right = DirectX::XMVectorSetY(right, 0.0f);
			right = DirectX::XMVector3Normalize(right);
		}

		forward = DirectX::XMVector3Cross(normal, right);

		DirectX::XMFLOAT3 xmf, xmr, xmn = tri->normal;

		DirectX::XMStoreFloat3(&xmf, forward);
		DirectX::XMStoreFloat3(&xmr, right);
*/
		m_player.SetRotation(tri->normal);
		auto pos = m_player.GetPosition();
		pos.y = iPoint.y + 0.5f;
		m_player.SetPosition(pos);
	}
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

	static bool FollowPlayerPressedLastFrame = false;
	bool FollowPlayerPressed = wnd->IsKeyPressed(Input::SPACE);


	if (!FollowPlayerPressedLastFrame && FollowPlayerPressed)
	{
		m_isFollowingPlayer = !m_isFollowingPlayer;
		DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&m_player.GetPosition());
		m_distanceToPlayer = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(playerPos, DirectX::XMLoadFloat4(&m_camera.GetPosition()))));
	}

	FollowPlayerPressedLastFrame = FollowPlayerPressed;

	if (m_isFollowingPlayer)
	{
		const DirectX::XMFLOAT4 camDir = m_camera.GetDirectionVector();

		DirectX::XMVECTOR camPosDir = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&camDir), -1.0f);
		DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&m_player.GetPosition());

		DirectX::XMVECTOR camPos = DirectX::XMVectorAdd(playerPos, DirectX::XMVectorScale(camPosDir, m_distanceToPlayer));

		DirectX::XMFLOAT3 xmCamPos;
		DirectX::XMStoreFloat3(&xmCamPos, camPos);
		m_camera.SetPosition(xmCamPos);
	}

	DirectX::XMFLOAT3 translation(0, 0, 0);

	if (wnd->IsKeyPressed(Input::UP_ARROW) || mp.y < 0.1 * wnd->GetResolutionSize().y)
	{
		translation.z += CAMERA_XZ_SPEED * (float)dt;
		m_isFollowingPlayer = false;
	}
	if (wnd->IsKeyPressed(Input::DOWN_ARROW) || mp.y > wnd->GetResolutionSize().y * 0.9)
	{
		translation.z -= CAMERA_XZ_SPEED * (float)dt;
		m_isFollowingPlayer = false;
	}
	if (wnd->IsKeyPressed(Input::RIGHT_ARROW) || mp.x > wnd->GetResolutionSize().x * 0.9)
	{
		translation.x += CAMERA_XZ_SPEED * (float)dt;
		m_isFollowingPlayer = false;
	}
	if (wnd->IsKeyPressed(Input::LEFT_ARROW) || mp.x < wnd->GetResolutionSize().x * 0.1)
	{
		translation.x -= CAMERA_XZ_SPEED * (float)dt;
		m_isFollowingPlayer = false;
	}
	if (wnd->IsKeyPressed(Input::V))
	{
		translation.y += CAMERA_XZ_SPEED * (float)dt;
		m_isFollowingPlayer = false;
	}
	if (wnd->IsKeyPressed(Input::C))
	{
		translation.y -= CAMERA_XZ_SPEED * (float)dt;
		m_isFollowingPlayer = false;
	}

	if (!m_isFollowingPlayer && !wnd->IsMousePressed(Input::MOUSE_CODE::MBUTTON))
		m_camera.Translate(translation);
	
	INT scrollDelta;
	if (scrollDelta = wnd->GetMouseWheelDelta(Input::VERTICAL))
	{
		DirectX::XMFLOAT2 mousePos, center;
		mousePos.x = (float)mp.x;
		mousePos.y = (float)mp.y;
		center.x = (float)m_mouseReferencePosition.x;
		center.y = (float)m_mouseReferencePosition.y;

		DirectX::XMFLOAT3 worldPos;
		if (Renderer::GetInstance()->GetMousePicking(worldPos))
		{
			if (m_isFollowingPlayer)
				worldPos = m_player.GetPosition();

			DirectX::XMFLOAT4 camPos = m_camera.GetPosition();
			camPos.w = 0.0f;

			DirectX::XMVECTOR direction = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&worldPos), DirectX::XMLoadFloat4(&camPos)));

			direction = DirectX::XMVectorScale(direction, scrollDelta * (float)CAMERA_ZOOM_SPEED);

			DirectX::XMFLOAT3 trans;
			DirectX::XMStoreFloat3(&trans, direction);
			
			camPos.x += trans.x;
			camPos.y += trans.y;
			camPos.z += trans.z;
			m_camera.SetPosition(camPos.x, camPos.y, camPos.z);

			if (m_isFollowingPlayer)
			{
				DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&m_player.GetPosition());
				m_distanceToPlayer = DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(playerPos, DirectX::XMLoadFloat4(&m_camera.GetPosition()))));
			}

		}
		else
		{
			m_camera.Translate(0, 0, (float)CAMERA_ZOOM_SPEED * scrollDelta, false);
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
			if (m_isFollowingPlayer)
			{
				_sWorldPosRef = m_player.GetPosition();
				PressedLastFrame = true;
			}

		}
		else
		{
			if (m_isFollowingPlayer)
			{
				_sWorldPosRef = m_player.GetPosition();
			}
			float mouseDeltaX = (float)mp.x - (float)_sMouseRef.x;
			float mouseDeltaY = (float)mp.y - (float)_sMouseRef.y;

			DirectX::XMFLOAT2 camRotation(DirectX::XMConvertToRadians(mouseDeltaX) * (float)dt * (float)MOUSE_SESITIVITY_X * 2.0f, DirectX::XMConvertToRadians(mouseDeltaY) * (float)dt * (float)MOUSE_SESITIVITY_Y * 2.0f);

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

}

#include "water_texture.h"

void Game::_createWaterTexture()
{
	DirectX::XMFLOAT4 waterArr[WATER_WIDTH * WATER_HEIGHT];

	const char * data = header_data;

	for (int i = 0; i < WATER_WIDTH * WATER_HEIGHT; i++)
	{
		int pixel[3];
		HEADER_PIXEL(data, pixel);

		waterArr[i].x = (float)pixel[0] / 256.0f;
		waterArr[i].y = (float)pixel[1] / 256.0f;
		waterArr[i].z = (float)pixel[2] / 256.0f;
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

void Game::_createWorld()
{
	std::cout << "****************************************\nCreatingWorld\n\n";

	Timer t;
	t.Start();
	_createTerrain();
	std::cout << std::endl;
	_buildTrees();
	std::cout << std::endl;
	_createBlockedTrianglesAndWaypoints();
	std::cout << std::endl;
	_placeTrianglesInTree();
	std::cout << std::endl;
	_offsetWaypoints();
	std::cout << std::endl;
	_cleanWaypoints();
	std::cout << std::endl;
	_connectWaypoints();
	std::cout << std::endl;
	_generateWorldEdges();
	std::cout << std::endl;

	if (DRAW_TRIANGLES)
	{
		_createViewableTriangles();
		std::cout << std::endl;
	}

	if (DRAW_WAYPOINT)
	{
		_createViewableWaypoints();
		std::cout << std::endl;
	}

	if (DRAW_CONNECTIONS && !m_connectionMesh.empty())
	{
		_createViewableConnections();
		std::cout << std::endl;
	}

	std::cout << "Total Time: " << t.Stop() << " Seconds\n";
	std::cout << "World Created\n****************************************\n";
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

		for (size_t k = i; k < i + 3; k++)
			DirectX::XMStoreFloat4A(&verts[k].Normal, normal);

	}
	m_playerMesh = verts;
	/* CUBE _ END*/

	/* PLANE */
	m_water.SetScale((float)TERRAIN_SIZE - 1.5f, 1.0f, (float)TERRAIN_SIZE - 1.5f);
	m_water.SetPickable(1);

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
	m_water.SetColor(1.0f, 1.0f, 1.0f, 0.8f);
	
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

	//m_player.SetPosition(5, 500, 5);
	m_camera.SetDirection(1, -2, 1);

	const DirectX::XMFLOAT4 camDir = m_camera.GetDirectionVector();

	DirectX::XMVECTOR camPosDir = DirectX::XMVectorScale(DirectX::XMLoadFloat4(&camDir), -1.0f);
	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&m_player.GetPosition());
	DirectX::XMVECTOR camPos = DirectX::XMVectorAdd(playerPos, DirectX::XMVectorScale(camPosDir, TERRAIN_SIZE));

	DirectX::XMFLOAT3 xmCamPos;
	DirectX::XMStoreFloat3(&xmCamPos, camPos);
	m_camera.SetPosition(xmCamPos);
}

void Game::_createTerrain()
{
	std::cout << "Creating Terrain... ";

	Timer t;
	m_noise = (float)(rand() % NOISE_RAND) + NOISE_MIN;

	m_maxHeight = MAX_HEIGHT * m_noise;
	m_minHeight = abs(MIN_HEIGHT * m_noise);

	t.Start();
	m_terrainMesh = m_terrainCreator.CreateTerrainFromFloatList2(
		m_diamondSquare.CreateDiamondSquare(TERRAIN_SIZE, TERRAIN_SIZE - 1, m_noise, MIN_HEIGHT, MAX_HEIGHT, 2),
		TERRAIN_SIZE,
		m_terrainTexture,
		m_terrainTex2D,
		m_edgeMeshes
	);

	m_terrain.SetVertices(&m_terrainMesh);
	m_terrain.SetTexture(m_terrainTexture);

	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_buildTrees()
{
	std::cout << "Building Trees... ";

	Timer t;

	DirectX::XMFLOAT2 worldStart = DirectX::XMFLOAT2(m_terrainMesh[0].Position.x, m_terrainMesh[0].Position.z);

	// Calculate tree depth and build quad tree for triangles
	int reversedTreeSize = 2;
	int treeLevels = 0;
	t.Start();
	while (reversedTreeSize < TERRAIN_SIZE)
	{
		treeLevels++;
		reversedTreeSize *= 2;
	}

	std::cout << "Depth: " << treeLevels << "... ";
	m_blockedTriangleTree.BuildTree(treeLevels, TERRAIN_SIZE, worldStart);
	m_unblockedTriangleTree.BuildTree(treeLevels, TERRAIN_SIZE, worldStart);
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_createBlockedTrianglesAndWaypoints()
{
	std::cout << "Detecting blocked/unblocked triangles and placing waypoints...\n";
	Timer t;
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0, -1, 0, 0);
	t.Start();
	for (int i = 0; i < m_terrainMesh.size(); i += 3)
	{
		DirectX::XMFLOAT4A v0, v1, v2;
		v0 = m_terrainMesh[i].Position;
		v1 = m_terrainMesh[i + 1].Position;
		v2 = m_terrainMesh[i + 2].Position;

		DirectX::XMVECTOR e0 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v1), DirectX::XMLoadFloat4A(&v0));
		DirectX::XMVECTOR e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat4A(&v2), DirectX::XMLoadFloat4A(&v0));
		DirectX::XMVECTOR normal = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(e0, e1));

		DirectX::XMStoreFloat4A(&m_terrainMesh[i].Normal, normal);
		DirectX::XMStoreFloat4A(&m_terrainMesh[i + 1].Normal, normal);
		DirectX::XMStoreFloat4A(&m_terrainMesh[i + 2].Normal, normal);


		float dot = fabs(DirectX::XMVectorGetX(DirectX::XMVector3Dot(normal, up)));
		bool createBlockedTriangleLocation = false;
		if (v0.y < m_terrainCreator.WATER_START || v1.y < m_terrainCreator.WATER_START || v2.y < m_terrainCreator.WATER_START)
			createBlockedTriangleLocation = true;

		if (dot < m_terrainCreator.UNWALKABLE_SURFACE || createBlockedTriangleLocation)
		{
			createBlockedTriangleLocation = true;

			DirectX::XMFLOAT2 middle;
			middle.x = (m_terrainMesh[i].Position.x + m_terrainMesh[i + 1].Position.x + m_terrainMesh[i + 2].Position.x) / 3;
			middle.y = (m_terrainMesh[i].Position.z + m_terrainMesh[i + 1].Position.z + m_terrainMesh[i + 2].Position.z) / 3;

			DirectX::XMVECTOR vMiddle = DirectX::XMLoadFloat2(&middle);

			for (int k = i; k < i + 3; k++)
			{
				DirectX::XMFLOAT4A pos = m_terrainMesh[k].Position;
				DirectX::XMFLOAT2 pos2D = { pos.x, pos.z };

				DirectX::XMVECTOR vPos2D = DirectX::XMLoadFloat2(&pos2D);
				DirectX::XMVECTOR vDir = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(vPos2D, vMiddle));

				DirectX::XMFLOAT2 dir;
				DirectX::XMStoreFloat2(&dir, vDir);

				Waypoint wp(pos.x, pos.z);
				wp.SetHeightVal(pos.y);

				long int xPos = (long)(pos.x + 0.5f);
				long int yPos = (long)(pos.z + 0.5f);

				long int key = xPos + yPos * (TERRAIN_SIZE);

				auto it = m_waypoints.find(key);

				if (it == m_waypoints.end())
					m_waypoints.insert(std::make_pair(key, wp));
			}
		}
		if (createBlockedTriangleLocation)
			m_blockedTriangles.push_back(new Triangle(v0, v1, v2));
		else
		{
			m_unblockedTriangles.push_back(new Triangle(v0, v1, v2));
		}
	}

	std::cout << "Waypoints: " << m_waypoints.size() << "...\n";
	std::cout << "Blocked tri: " << m_blockedTriangles.size() << "...\n";
	std::cout << "Unblocked tri:" << m_unblockedTriangles.size() << "...\nTime: " << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_cleanWaypoints()
{
	std::cout << "Cleaning waypoints Iteration1... ";
	Timer t;
	t.Start();

	/*
	 * First iteration - Initial blocked cleanup
	 */
	std::vector<int> keys;
	for (int y = 1; y < TERRAIN_SIZE - 1; y++)
	{
		for (int x = 1; x < TERRAIN_SIZE - 1; x++)
		{
			int key = x + y * TERRAIN_SIZE;
			auto it = m_waypoints.find(key);
			if (it != m_waypoints.end())
			{
				// North
				int innerKey = x + (y - 1) * TERRAIN_SIZE;
				it = m_waypoints.find(innerKey);

				if (it != m_waypoints.end())
				{
					// East
					innerKey = (x + 1) + y * TERRAIN_SIZE;
					it = m_waypoints.find(innerKey);

					if (it != m_waypoints.end())
					{
						// South
						innerKey = x + (y + 1) * TERRAIN_SIZE;
						it = m_waypoints.find(innerKey);

						if (it != m_waypoints.end())
						{
							// West
							innerKey = (x - 1) + y * TERRAIN_SIZE;
							it = m_waypoints.find(innerKey);

							if (it != m_waypoints.end())
								keys.push_back(key);
						}
					}
				}
			}
		}
	}

	for (int i = 0; i < TERRAIN_SIZE; i++)
	{
		m_waypoints.erase(i);
		m_waypoints.erase(i * TERRAIN_SIZE);
		m_waypoints.erase(i * TERRAIN_SIZE - 1);
		m_waypoints.erase(TERRAIN_SIZE - i);
	}

	// Remove the flagged waypoints
	int vectorSize = keys.size();
	for (int i = 0; i < vectorSize; i++)
		m_waypoints.erase(keys[i]);

	/*
	 * Second iteration - Remove waypoints that follow each other in the x-axis
	 */
	// Post process waypoints and remove waypoints in a line
	// Extract all waypoint keys
	std::vector<int> keysSecondIteration(m_waypoints.size());
	int waypointPositionCounter = 0;
	for (auto & wp : m_waypoints)
		keysSecondIteration[waypointPositionCounter++] = wp.first;

	vectorSize = keysSecondIteration.size();
	for (int i = 0; i < vectorSize; i++)
	{
		int removeThisManyWaypoints = 1;
		bool keysFollowEachother = true;

		while (keysFollowEachother)
		{
			int index = i + removeThisManyWaypoints;
			if (index == vectorSize)
			{
				removeThisManyWaypoints--;
				for (int j = 1; j < removeThisManyWaypoints; j++)
				{
					i++;
					m_waypoints.erase(keysSecondIteration[i]);
				}
				break;
			}
			// Check if the keys are in a line
			DirectX::XMFLOAT2 halfwayToNextTrianglePoint;
			halfwayToNextTrianglePoint.x = (keysSecondIteration[index - 1] % TERRAIN_SIZE) + (keysSecondIteration[index] % TERRAIN_SIZE);
			halfwayToNextTrianglePoint.x *= 0.5;
			halfwayToNextTrianglePoint.y = keysSecondIteration[index] / TERRAIN_SIZE;
			Triangle * ptr = m_blockedTriangleTree.PointInsideTriangle(halfwayToNextTrianglePoint, true);
			if (keysSecondIteration[i] != keysSecondIteration[index] - removeThisManyWaypoints &&
				ptr != nullptr)
			{
				keysFollowEachother = false;
				removeThisManyWaypoints--;

				for (int j = 1; j < removeThisManyWaypoints; j++)
				{
					i++;
					m_waypoints.erase(keysSecondIteration[i]);
				}
			}
			else if (ptr == nullptr)
			{
				keysFollowEachother = false;
				removeThisManyWaypoints--;

				for (int j = 1; j < removeThisManyWaypoints; j++)
				{
					i++;
					m_waypoints.erase(keysSecondIteration[i]);
				}
			}
			else
				removeThisManyWaypoints++;
		}
	}

	/*
	 * Third iteration - Remove waypoints that follow each other in the y-axis
	 */
	std::vector<int> keysThirdIteration(m_waypoints.size());
	waypointPositionCounter = 0;
	for (auto & wp : m_waypoints)
		keysThirdIteration[waypointPositionCounter++] = wp.first;

	vectorSize = keysThirdIteration.size();
	for (int i = 0; i < vectorSize; i++)
	{
		int removeThisManyWaypoints = 1;
		int lastWaypointKey = keysThirdIteration[i];
		bool keysFollowEachother = true;
		std::vector<int> keysToRemove;

		while (keysFollowEachother)
		{
			if (i == vectorSize)
			{
				int removeSize = keysToRemove.size() - 1;
				for (int j = 0; j < removeSize; j++)
					m_waypoints.erase(keysToRemove[j]);

				break;
			}

			int nextWaypointKey = keysThirdIteration[i] + removeThisManyWaypoints * TERRAIN_SIZE;
			auto nextWpIt = m_waypoints.find(nextWaypointKey);

			if (nextWpIt != m_waypoints.end())
			{
				DirectX::XMFLOAT2 halfwayToNextTrianglePoint;
				halfwayToNextTrianglePoint.x = nextWaypointKey % TERRAIN_SIZE;
				halfwayToNextTrianglePoint.y = (lastWaypointKey / TERRAIN_SIZE) + (nextWaypointKey / TERRAIN_SIZE);
				halfwayToNextTrianglePoint.y *= 0.5;
				Triangle * ptr = m_blockedTriangleTree.PointInsideTriangle(halfwayToNextTrianglePoint, true);
				if (ptr == nullptr)
				{
					keysFollowEachother = false;

					int removeSize = keysToRemove.size() - 1;
					for (int j = 0; j < removeSize; j++)
						m_waypoints.erase(keysToRemove[j]);
				}
				else
				{
					removeThisManyWaypoints++;
					lastWaypointKey = nextWaypointKey;
					keysToRemove.push_back(nextWaypointKey);
				}
			}
			else
			{
				keysFollowEachother = false;

				int removeSize = keysToRemove.size() - 1;
				for (int j = 0; j < removeSize; j++)
					m_waypoints.erase(keysToRemove[j]);
			}
		}
	}

	/*
	 * Fourth iteration - Remove waypoints that follow each other in one of the diagonals
	 */
	std::vector<int> keysFourthIteration(m_waypoints.size());
	waypointPositionCounter = 0;
	for (auto & wp : m_waypoints)
		keysFourthIteration[waypointPositionCounter++] = wp.first;

	vectorSize = keysFourthIteration.size();
	for (int i = 0; i < vectorSize; i++)
	{
		int removeThisManyWaypoints = 1;
		int lastWaypointKey = keysFourthIteration[i];
		bool keysFollowEachother = true;
		std::vector<int> keysToRemove;

		while (keysFollowEachother)
		{
			if (i == vectorSize)
			{
				int removeSize = keysToRemove.size() - 1;
				for (int j = 0; j < removeSize; j++)
					m_waypoints.erase(keysToRemove[j]);

				break;
			}

			// Diagonal movement forward direction
			int nextWaypointKey = keysFourthIteration[i] + removeThisManyWaypoints + removeThisManyWaypoints * TERRAIN_SIZE;
			auto nextWpIt = m_waypoints.find(nextWaypointKey);

			if (nextWpIt != m_waypoints.end())
			{
				DirectX::XMFLOAT2 halfwayToNextTrianglePoint;
				halfwayToNextTrianglePoint.x = (lastWaypointKey % TERRAIN_SIZE) + (nextWaypointKey % TERRAIN_SIZE);
				halfwayToNextTrianglePoint.x *= 0.5;
				halfwayToNextTrianglePoint.y = (lastWaypointKey / TERRAIN_SIZE) + (nextWaypointKey / TERRAIN_SIZE);
				halfwayToNextTrianglePoint.y *= 0.5;
				Triangle * ptr = m_blockedTriangleTree.PointInsideTriangle(halfwayToNextTrianglePoint, true);
				if (ptr == nullptr)
				{
					keysFollowEachother = false;

					int removeSize = keysToRemove.size() - 1;
					for (int j = 0; j < removeSize; j++)
						m_waypoints.erase(keysToRemove[j]);
				}
				else
				{
					removeThisManyWaypoints++;
					lastWaypointKey = nextWaypointKey;
					keysToRemove.push_back(nextWaypointKey);
				}
			}
			else
			{
				keysFollowEachother = false;

				int removeSize = keysToRemove.size() - 1;
				for (int j = 0; j < removeSize; j++)
					m_waypoints.erase(keysToRemove[j]);
			}
		}
	}

	/*
	 * Fifth iteration - Remove waypoints that follow each other in the other diagonal
	 */
	std::vector<int> keysFifthIteration(m_waypoints.size());
	waypointPositionCounter = 0;
	for (auto & wp : m_waypoints)
		keysFifthIteration[waypointPositionCounter++] = wp.first;

	vectorSize = keysFifthIteration.size();
	for (int i = 0; i < vectorSize; i++)
	{
		int removeThisManyWaypoints = 1;
		int lastWaypointKey = keysFifthIteration[i];
		bool keysFollowEachother = true;
		std::vector<int> keysToRemove;

		while (keysFollowEachother)
		{
			if (i == vectorSize)
			{
				int removeSize = keysToRemove.size() - 1;
				for (int j = 0; j < removeSize; j++)
					m_waypoints.erase(keysToRemove[j]);

				break;
			}

			// Diagonal movement forward direction
			int nextWaypointKey = keysFifthIteration[i] - removeThisManyWaypoints + removeThisManyWaypoints * TERRAIN_SIZE;
			auto nextWpIt = m_waypoints.find(nextWaypointKey);

			if (nextWpIt != m_waypoints.end())
			{
				DirectX::XMFLOAT2 halfwayToNextTrianglePoint;
				halfwayToNextTrianglePoint.x = (lastWaypointKey % TERRAIN_SIZE) + (nextWaypointKey % TERRAIN_SIZE);
				halfwayToNextTrianglePoint.x *= 0.5;
				halfwayToNextTrianglePoint.y = (lastWaypointKey / TERRAIN_SIZE) + (nextWaypointKey / TERRAIN_SIZE);
				halfwayToNextTrianglePoint.y *= 0.5;
				Triangle * ptr = m_blockedTriangleTree.PointInsideTriangle(halfwayToNextTrianglePoint, true);
				if (ptr == nullptr)
				{
					keysFollowEachother = false;

					int removeSize = keysToRemove.size() - 1;
					for (int j = 0; j < removeSize; j++)
						m_waypoints.erase(keysToRemove[j]);
				}
				else
				{
					removeThisManyWaypoints++;
					lastWaypointKey = nextWaypointKey;
					keysToRemove.push_back(nextWaypointKey);
				}
			}
			else
			{
				keysFollowEachother = false;

				int removeSize = keysToRemove.size() - 1;
				for (int j = 0; j < removeSize; j++)
					m_waypoints.erase(keysToRemove[j]);
			}
		}
	}

	std::cout << "Waypoints : " << m_waypoints.size() << "... ";
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_placeTrianglesInTree()
{
	Timer t;
	std::cout << "Placeing triangles in trees... ";
	t.Start();
	m_blockedTriangleTree.PlaceObjects(m_blockedTriangles);
	m_unblockedTriangleTree.PlaceObjects(m_unblockedTriangles);
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_createViewableTriangles()
{
	std::cout << "Create drawables for tree triangles..\n";
	Timer t;
	t.Start();
	for (auto & t : m_blockedTriangles)
	{
		Vertex v;
		v.Normal = DirectX::XMFLOAT4A(t->normal.x, t->normal.y, t->normal.z, 0.0f);

		for (int i = 0; i < 3; i++)
		{
			v.Position = DirectX::XMFLOAT4A(t->points[i].x, t->points[i].y, t->points[i].z, 1.0f);
			m_blockedTrianglesVertices.push_back(v);
		}
	}
	for (auto & t : m_unblockedTriangles)
	{
		Vertex v;
		v.Normal = DirectX::XMFLOAT4A(t->normal.x, t->normal.y, t->normal.z, 0.0f);

		for (int i = 0; i < 3; i++)
		{
			v.Position = DirectX::XMFLOAT4A(t->points[i].x, t->points[i].y, t->points[i].z, 1.0f);
			m_unblockedTrianglesVertices.push_back(v);
		}
	}
	m_blockedtrianglesDraw.SetVertices(&m_blockedTrianglesVertices);
	m_blockedtrianglesDraw.SetColor(1.0f, 0.0f, 0.0f);
	m_blockedtrianglesDraw.Update();
	m_unblockedtrianglesDraw.SetVertices(&m_unblockedTrianglesVertices);
	m_unblockedtrianglesDraw.SetColor(0.0f, 1.0f, 0.0f);
	m_unblockedtrianglesDraw.Update();
	
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_createViewableWaypoints()
{
	std::cout << "Create drawables for Waypoints... ";
	m_wp = std::vector<Drawable>(m_waypoints.size());
	int counter = 0;
	Timer t;
	t.Start();
	for (auto & w : m_waypoints)
	{
		m_wp[counter].SetPosition(w.second.GetPosition().x, w.second.GetHeightVal(), w.second.GetPosition().y);
		m_wp[counter].SetVertices(&m_playerMesh);
		m_wp[counter].SetScale(0.1f, 0.1f, 0.1f);
		m_wp[counter].UseDepthBuffer(false);
		m_wp[counter].Update();
		m_wp[counter++].SetColor(0, 1, 1);
	}

	
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_offsetWaypoints()
{
	Timer t;
	std::cout << "Prepareing Waypoints for connections... ";

	t.Start();

	for (auto & wp : m_waypoints)
	{
		DirectX::XMFLOAT2 pRef = wp.second.GetPosition();
		DirectX::XMFLOAT2 offset(0.0f, 0.0f);
		
		int xRef = (pRef.x + 0.5f);
		int yRef = (pRef.y + 0.5f);

		// North
		DirectX::XMINT2 north(xRef, yRef - 1);
		auto itNorth = m_waypoints.find((north.x + north.y * TERRAIN_SIZE));
		bool hasNorth = itNorth != m_waypoints.end();
		if (hasNorth)
		{
			offset.y += 0.5f;
		}

		// East
		DirectX::XMINT2 east(xRef + 1, yRef);
		auto itEast = m_waypoints.find((east.x + east.y * TERRAIN_SIZE));
		bool hasEast = itEast != m_waypoints.end();
		if (hasEast)
		{
			offset.x -= 0.5f;
		}

		// South
		DirectX::XMINT2 south(xRef, yRef + 1);
		auto itSouth = m_waypoints.find((south.x + south.y * TERRAIN_SIZE));
		bool hasSouth = itSouth != m_waypoints.end();
		if (hasSouth)
		{
			offset.y -= 0.5f;
		}

		// West
		DirectX::XMINT2 west(xRef - 1, yRef);
		auto itWest = m_waypoints.find((west.x + west.y * TERRAIN_SIZE));
		bool hasWest = itWest != m_waypoints.end();
		if (hasWest)
		{
			offset.x += 0.5f;
		}

		// North-East
		DirectX::XMINT2 northEast(xRef + 1, yRef - 1);
		auto itNorthEast = m_waypoints.find((northEast.x + northEast.y * TERRAIN_SIZE));
		bool hasNorthEast = itNorthEast != m_waypoints.end();
		if (hasNorthEast)
		{
			offset.y += 0.5f;
			offset.x -= 0.5f;
		}


		// South-East
		DirectX::XMINT2 southEast(xRef + 1, yRef + 1);
		auto itSouthEast = m_waypoints.find((southEast.x + southEast.y * TERRAIN_SIZE));
		bool hasSouthEast = itSouthEast != m_waypoints.end();
		if (hasSouthEast)
		{
			offset.y -= 0.5f;
			offset.x -= 0.5f;
		}


		// South-West
		DirectX::XMINT2 southWest(xRef - 1, yRef + 1);
		auto itSouthWest = m_waypoints.find((southWest.x + southWest.y * TERRAIN_SIZE));
		bool hasSouthWest = itSouthWest != m_waypoints.end();
		if (hasSouthWest)
		{
			offset.y -= 0.5f;
			offset.x += 0.5f;
		}

		// North-West
		DirectX::XMINT2 NorthWest(xRef + 1, yRef - 1);
		auto itNorthWest = m_waypoints.find((NorthWest.x + NorthWest.y * TERRAIN_SIZE));
		bool hasNorthWest = itNorthWest != m_waypoints.end();
		if (hasNorthWest)
		{
			offset.y += 0.5f;
			offset.x += 0.5f;
		}

		offset.x = std::clamp(offset.x, -0.5f, 0.5f);
		offset.y = std::clamp(offset.y, -0.5f, 0.5f);

		pRef.x += offset.x;
		pRef.y += offset.y;

		wp.second.SetPosition(pRef.x, pRef.y);

	}


	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_connectWaypoints()
{
	// Connect Waypoints
	std::cout << "Connecting Waypoints... ";
	Timer t;
	int counter = 0;
	Vertex v1, v2;
	DirectX::XMFLOAT4A n = { 0.0f, 1.0f, 0.0f, 0.0f };
	v1.Normal = n;
	v2.Normal = n;
	v1.Position.w = 1.0f;
	v2.Position.w = 1.0f;
	t.Start();
	std::cout << std::endl;
	long int counter2 = 0;

	long int totalSize = m_waypoints.size();

	for (auto & wp1 : m_waypoints)
	{	
		std::cout << "\r" << std::to_string(((double)counter2++ / totalSize) * 100.0) << "%";
		
		for (auto & wp2 : m_waypoints)
		{
			if (wp1.first == wp2.first)
				continue;

			DirectX::XMFLOAT2 p1, p2;

			DirectX::XMFLOAT2 dummy;
			Triangle * tri = m_blockedTriangleTree.LineIntersectionTriangle(p1 = wp1.second.GetPosition(), p2 = wp2.second.GetPosition(), true, dummy);


			if (tri == nullptr)
			{

				v1.Position.x = p1.x;
				v1.Position.y = wp1.second.GetHeightVal();
				v1.Position.z = p1.y;

				v2.Position.x = p2.x;
				v2.Position.y = wp2.second.GetHeightVal();
				v2.Position.z = p2.y;


				if (wp1.second.Connect(&m_waypoints[wp2.first]))
				{
					m_connectionMesh.push_back(v1);
					m_connectionMesh.push_back(v2);
					counter++;
					wp2.second.ForceConnection(&m_waypoints[wp1.first]);
				}
			}
		}
	}
	std::cout << "\nConnection(s): " << counter << "... ";
	std::cout << t.Stop(Timer::MILLISECONDS) << "ms\n";

}

void Game::_createViewableConnections()
{
	m_connections.SetVertices(&m_connectionMesh);
	m_connections.SetColor(1, 0, 1);
	m_connections.SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	m_connections.UseDepthBuffer(false);
}

void Game::_generateWorldEdges()
{
	std::cout << "Generating world edges... ";
	Timer t;
	t.Start();
	for (int i = 0; i < 4; i++)
	{
		int size = (int)m_edgeMeshes[i].size();
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
			ver.Position.y = MIN_HEIGHT * m_noise;

			m_edgeMeshes[i].insert(m_edgeMeshes[i].begin() + v, ver);

			size++;
		}

		m_edges[i].SetVertices(&m_edgeMeshes[i]);
		m_edges[i].SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		m_edges[i].SetColor(0.1, 0.2f, 0.1f, 1.0f);
		m_edges[i].SetPickable(true);
	}
	
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}
