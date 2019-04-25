#include "waypointGenerationPCH.h"
#include "Game.h"
#include "../Rendering/Rendering/Renderer.h"
#include "GPUQuadTree/GPUQuadTree.h"
#include "Pathfinding/Pathfinding.h"

#include <time.h>
#include <algorithm>

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
	m_pathfinding.InitiateThreadpool(4);
	m_pathId = UINT64_MAX;
	m_player.SetSpeed(25);

}

Game::~Game()
{
	m_terrainTex2D->Release();
	m_terrainTexture->Release();
	m_waterTex2D->Release();
	m_waterTexture->Release();

	m_pathfinding.Terminate();

	for (auto & t : m_blockedTriangles)
		delete t;
	for (auto & t : m_unblockedTriangles)
		delete t;

	m_blockedTriangles.clear();
	m_unblockedTriangles.clear();
	
	
	m_wpDraw.clear();
}

void Game::Update(double dt)
{
	if (m_pathId != UINT64_MAX)
		if (m_pathfinding.IsPathDone(m_pathId))
		{
			std::vector<DirectX::XMFLOAT2> path = m_pathfinding.GetPath(m_pathId);
			if (!path.empty())
				m_player.SetPath(path);
			m_pathId = UINT64_MAX;
		}

	if (Window::GetInstance()->IsKeyPressed(Input::KEY_CODE::S))
		m_player.SetPath(std::vector<DirectX::XMFLOAT2>());

	_playerFixYPosition(dt);
	_cameraControl(dt);

	m_camera.Update();
	m_player.Update();
	m_terrain.Update();

	m_waterCounter += dt * 0.1;

	m_waterUV.x = (float)m_waterCounter;
	m_waterUV.y = m_waterUV.x;

	m_water.SetUVOffset(m_waterUV.x, m_waterUV.y);
	m_water.Update();

	for (int i = 0; i < 4; i++)
		m_edges[i].Update();
	

	for (auto & b : m_buildings)
		b.Update();

	m_coinTrans += dt;
	while (m_coinTrans > DirectX::XM_2PI)
		m_coinTrans -= DirectX::XM_2PI;

	double t = cos(m_coinTrans);

	for (auto & c : m_coins)
	{
		c.Translate(0, t * dt, 0);
		c.Rotate(0, dt, 0);
		c.Update();
	}

	std::vector<Drawable*> col = m_unblockedTriangleTree.DrawableIntersects(&m_player);

	for (auto & c : col)
		c->SetActive(false);


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

	for (auto & d : m_wpDraw)
		d.second.Draw();

	for (auto & c : m_coins)
		c.Draw();

	m_water.Draw();
}

bool Game::GameOver()
{
	for (auto & c : m_coins)
		if (c.IsActive())
			return false;
	return true;
}

void Game::_playerFixYPosition(double dt)
{
	Window * wnd = Window::GetInstance();
	m_player.UnitUpdate(dt, &m_blockedTriangleTree);

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

	if (!m_isFollowingPlayer)
	{
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

	/*if (wnd->IsMousePressed(Input::MOUSE_CODE::LBUTTON))
	{
		DirectX::XMFLOAT3 worldPos;
		if (Renderer::GetInstance()->GetMousePicking(worldPos))
		{
			m_player.SetPosition(worldPos.x, worldPos.y + 0.5f, worldPos.z);
		}
	}*/

	if (wnd->IsMousePressed(Input::MOUSE_CODE::RBUTTON))
	{
		DirectX::XMFLOAT3 worldPos;
		if (Renderer::GetInstance()->GetMousePicking(worldPos) && m_pathId == UINT64_MAX)
		{
			m_pathId = m_pathfinding.RequestPath(m_player.GetPosition(), worldPos, m_blockedTriangleTree);	
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
	std::cout << "Terrain Size: " << TERRAIN_SIZE << std::endl;
	Timer t;
	t.Start();
	_createTerrain();
	std::cout << std::endl;
	_buildTrees();
	std::cout << std::endl;
	_createBlockedTriangles();
	std::cout << std::endl;
	_placeTrianglesInTree();
	std::cout << std::endl;
	_smoothUnorthodoxShapes();
	std::cout << std::endl;
	_createWaypoints();
	std::cout << std::endl;
	_offsetWaypoints();
	std::cout << std::endl;
	_cleanWaypoints();
	std::cout << std::endl;
	_connectWaypoints();
	std::cout << std::endl;
	_generateWorldEdges();
	std::cout << std::endl;
	_creatingCoins();
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

void Game::_createBlockedTriangles()
{
	std::cout << "Detecting blocked/unblocked triangles...\n";
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
		}
		if (createBlockedTriangleLocation)
			m_blockedTriangles.push_back(new Triangle(v0, v1, v2));
		else
			m_unblockedTriangles.push_back(new Triangle(v0, v1, v2));
	}

	std::cout << "Blocked tri: " << m_blockedTriangles.size() << "...\n";
	std::cout << "Unblocked tri:" << m_unblockedTriangles.size() << "...\nTime: " << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_createWaypoints()
{
	std::cout << "Creating waypoints...\n";
	Timer t;
	t.Start();
	
	int vectorSize = m_blockedTriangles.size();
	for (int i = 0; i < vectorSize; i++)
	{
		DirectX::XMFLOAT3 points[3];
		memcpy(points, m_blockedTriangles[i]->points, sizeof(float) * 9);

		for (int j = 0; j < 3; j++)
		{
			long int xPos = (long)(points[j].x + 0.5f);
			long int yPos = (long)(points[j].z + 0.5f);
			long int key = xPos + yPos * TERRAIN_SIZE;

			auto it = m_waypoints.find(key);

			if (it == m_waypoints.end())
			{
				Waypoint wp(points[j].x, points[j].z);
				wp.SetHeightVal(points[j].y);
				m_waypoints.insert(std::make_pair(key, wp));
			}
		}
	}

	std::cout << "Waypoints: " << m_waypoints.size() << "...\n";
	std::cout << "Time: " << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_cleanWaypoints()
{
	std::cout << "Cleaning waypoints blocked cleanup iteration... ";
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

	// Erase the waypoints at the maps edges
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

	std::cout << "Time: " << t.Stop(Timer::MILLISECONDS) << " ms\n";
	std::cout << "Waypoints : " << m_waypoints.size() << "...\n\n";
	std::cout << "X-axis iteration... ";

	/*
	 * Second iteration - Remove waypoints that follow each other in the x-axis
	 */
	// Post process waypoints and remove waypoints in a line
	// Extract all waypoint keys
	std::vector<int> keysSecondIteration(m_waypoints.size());
	int waypointPositionCounter = 0;
	std::vector<int> cleanBlocked;
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
	
	std::cout << "Time: " << t.Stop(Timer::MILLISECONDS) << " ms\n";
	std::cout << "Waypoints : " << m_waypoints.size() << "...\n\n";
	std::cout << "Y-axis iteration... ";

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

	std::cout << "Time: " << t.Stop(Timer::MILLISECONDS) << " ms\n";
	std::cout << "Waypoints : " << m_waypoints.size() << "...\n\n";
	std::cout << "Diagonal iteration... ";

	
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

	std::cout << "Time: " << t.Stop(Timer::MILLISECONDS) << " ms\n";
	std::cout << "Waypoints : " << m_waypoints.size() << "...\n\n";
	std::cout << "Cluster iteration... ";

	/*
	 * Sixth iteration - Cluster waypoints that are close to each other
	 */
	std::vector<int> keysSixthIteration(m_waypoints.size());
	waypointPositionCounter = 0;
	for (auto & wp : m_waypoints)
	{
		//keysSixthIteration[waypointPositionCounter++] = wp.first;
		int currentKey = wp.first;
		int indexAround[8];

		// North
		indexAround[0] = currentKey - TERRAIN_SIZE;
		// East
		indexAround[1] = currentKey + 1;
		// South
		indexAround[2] = currentKey + TERRAIN_SIZE;
		// West
		indexAround[3] = currentKey - 1;
		// Northeast
		indexAround[4] = currentKey - TERRAIN_SIZE + 1;
		// Southeast
		indexAround[5] = currentKey + TERRAIN_SIZE + 1;
		// Southwest
		indexAround[6] = currentKey + TERRAIN_SIZE - 1;
		// Northwest
		indexAround[7] = currentKey - TERRAIN_SIZE - 1;

		for (int j = 0; j < 8; j++)
		{
			auto wpIt = m_waypoints.find(indexAround[j]);
			if (wpIt != m_waypoints.end())
			{
				DirectX::XMFLOAT2 point1 = wp.second.GetPosition();
				DirectX::XMFLOAT2 point2 = wpIt->second.GetPosition();

				if (m_unblockedTriangleTree.PointInsideTriangle(point1, true) == m_unblockedTriangleTree.PointInsideTriangle(point2, true))
					keysSixthIteration.push_back(wpIt->first);
			}
		}
	}

	vectorSize = keysSixthIteration.size();
	for (int i = 0; i < vectorSize; i++)
		m_waypoints.erase(keysSixthIteration[i]);
	
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
	std::cout << "Waypoints : " << m_waypoints.size() << "...\n";
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

void Game::_smoothUnorthodoxShapes()
{
	Timer t;
	std::cout << "Smoothing unorthodox shapes... ";
	t.Start();

	for (auto & tri : m_unblockedTriangles)
	{
		// Check for blocked triangles in three directions
		// If all three are blocked then block this one
		for (int i = 0; i < 3; i++)
		{
			// Calculate triangle center
			DirectX::XMVECTOR p0, p1, p2;
			p0 = DirectX::XMLoadFloat3(&tri->points[0]);
			p1 = DirectX::XMLoadFloat3(&tri->points[1]);
			p2 = DirectX::XMLoadFloat3(&tri->points[2]);
			DirectX::XMVECTOR result = DirectX::XMVectorAdd(DirectX::XMVectorAdd(p0, p1), p2);
			result = DirectX::XMVectorScale(result, 0.3333333f);

			DirectX::XMFLOAT3 intermediateFloat;
			DirectX::XMFLOAT2 blockedCheckPoints[4];
			DirectX::XMStoreFloat3(&intermediateFloat, result);
			blockedCheckPoints[0] = DirectX::XMFLOAT2(intermediateFloat.x + 0.5f, intermediateFloat.z);
			blockedCheckPoints[1] = DirectX::XMFLOAT2(intermediateFloat.x - 0.5f, intermediateFloat.z);
			blockedCheckPoints[2] = DirectX::XMFLOAT2(intermediateFloat.x, intermediateFloat.z + 0.5f);
			blockedCheckPoints[3] = DirectX::XMFLOAT2(intermediateFloat.x, intermediateFloat.z - 0.5f);

			int directionCounter = 0;
			for (int i = 0; i < 4; i++)
			{
				if (m_blockedTriangleTree.PointInsideTriangle(blockedCheckPoints[i], true) != nullptr)
				{
					directionCounter++;
				}
			}
			if (directionCounter > 2)
			{
				Triangle * t = new Triangle(tri->points[0], tri->points[1], tri->points[2]);
				m_blockedTriangles.push_back(t);
				m_blockedTriangleTree.AddObject(t);
			}
		}
	}
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
	//m_wp = std::vector<Drawable>(m_waypoints.size());
	//int counter = 0;
	Timer t;
	t.Start();
	for (auto & w : m_waypoints)
	{
		m_wpDraw.insert(std::make_pair(&m_waypoints[w.first], Drawable()));
		m_wpDraw[&m_waypoints[w.first]].SetPosition(w.second.GetPosition().x, w.second.GetHeightVal(), w.second.GetPosition().y);
		m_wpDraw[&m_waypoints[w.first]].SetVertices(&m_playerMesh);
		m_wpDraw[&m_waypoints[w.first]].SetScale(0.1f, 0.1f, 0.1f);
		m_wpDraw[&m_waypoints[w.first]].UseDepthBuffer(false);
		m_wpDraw[&m_waypoints[w.first]].Update();
		m_wpDraw[&m_waypoints[w.first]].SetColor(0, 1, 1);
	}

	
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}

void Game::_offsetWaypoints()
{
	const float OFFSET = 1.0f;


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
			offset.y += OFFSET;
		}

		// East
		DirectX::XMINT2 east(xRef + 1, yRef);
		auto itEast = m_waypoints.find((east.x + east.y * TERRAIN_SIZE));
		bool hasEast = itEast != m_waypoints.end();
		if (hasEast)
		{
			offset.x -= OFFSET;
		}

		// South
		DirectX::XMINT2 south(xRef, yRef + 1);
		auto itSouth = m_waypoints.find((south.x + south.y * TERRAIN_SIZE));
		bool hasSouth = itSouth != m_waypoints.end();
		if (hasSouth)
		{
			offset.y -= OFFSET;
		}

		// West
		DirectX::XMINT2 west(xRef - 1, yRef);
		auto itWest = m_waypoints.find((west.x + west.y * TERRAIN_SIZE));
		bool hasWest = itWest != m_waypoints.end();
		if (hasWest)
		{
			offset.x += OFFSET;
		}

		// North-East
		DirectX::XMINT2 northEast(xRef + 1, yRef - 1);
		auto itNorthEast = m_waypoints.find((northEast.x + northEast.y * TERRAIN_SIZE));
		bool hasNorthEast = itNorthEast != m_waypoints.end();
		if (hasNorthEast)
		{
			offset.y += OFFSET;
			offset.x -= OFFSET;
		}


		// South-East
		DirectX::XMINT2 southEast(xRef + 1, yRef + 1);
		auto itSouthEast = m_waypoints.find((southEast.x + southEast.y * TERRAIN_SIZE));
		bool hasSouthEast = itSouthEast != m_waypoints.end();
		if (hasSouthEast)
		{
			offset.y -= OFFSET;
			offset.x -= OFFSET;
		}


		// South-West
		DirectX::XMINT2 southWest(xRef - 1, yRef + 1);
		auto itSouthWest = m_waypoints.find((southWest.x + southWest.y * TERRAIN_SIZE));
		bool hasSouthWest = itSouthWest != m_waypoints.end();
		if (hasSouthWest)
		{
			offset.y -= OFFSET;
			offset.x += OFFSET;
		}

		// North-West
		DirectX::XMINT2 NorthWest(xRef + 1, yRef - 1);
		auto itNorthWest = m_waypoints.find((NorthWest.x + NorthWest.y * TERRAIN_SIZE));
		bool hasNorthWest = itNorthWest != m_waypoints.end();
		if (hasNorthWest)
		{
			offset.y += OFFSET;
			offset.x += OFFSET;
		}

		offset.x = std::clamp(offset.x, -OFFSET, OFFSET);
		offset.y = std::clamp(offset.y, -OFFSET, OFFSET);

		pRef.x += offset.x;
		pRef.y += offset.y;

		wp.second.SetPosition(pRef.x, pRef.y);

	}


	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}
#include <fstream>
void Game::_connectWaypoints()
{
	static const UINT TREE_SIZE = 1024 * 1024 * 32;

	// Connect Waypoints
	std::cout << "Connecting Waypoints... ";
	Timer t;
	t.Start();
	int counter = 0;

	struct gpuWaypoint
	{
		UINT key = 0;
		UINT nrOfConnections = 0;
		DirectX::XMFLOAT2 pos;
		UINT connections[256];
	};

	GPUQuadTree gpuQuadTree;
	gpuQuadTree.BuildTree(0, 0, 7, TERRAIN_SIZE);

	size_t tSize = m_blockedTriangles.size();
	std::vector<Triangle> bTri(tSize);
	for (size_t i = 0; i < tSize; i++)
		bTri[i] = *m_blockedTriangles[i];

	gpuQuadTree.PlaceObjects(bTri);

	std::ofstream lol;
	lol.open("GPUTREE.txt");
	lol << gpuQuadTree.ToString();
	lol.close();

	const std::vector<GPUQuadrant> & qt = gpuQuadTree.GetQuadTree();
	size_t wpSize = m_waypoints.size();
	std::vector<gpuWaypoint> gpuWp(wpSize);

	size_t c = 0;

	for (auto & wp : m_waypoints)
	{
		gpuWaypoint gwp;
		memset(gwp.connections, 0, 256 * sizeof(UINT));

		gwp.key = wp.first;
		gwp.pos = wp.second.GetPosition();
		gpuWp[c++] = gwp;
	}

	ID3D11Buffer * quadTreeOld = nullptr;			// This can be read only
	ID3D11Buffer * trianglesOld = nullptr;			// This can be read only
	ID3D11Buffer * wpsOld = nullptr;				// This this needs RW

	ID3D11Buffer * offsetBuffer = nullptr;


	

	ID3D11ShaderResourceView * quadTree = nullptr;
	ID3D11ShaderResourceView * triangles = nullptr;
	ID3D11UnorderedAccessView * wps = nullptr;

	ID3D11Device * device = Renderer::GetInstance()->GetDevice();
	ID3D11DeviceContext * deviceContext = Renderer::GetInstance()->GetDeviceContext();
	ID3D11Query * async = nullptr;
	ID3D11Query * timeStamp = nullptr;
	ID3D11Query * timeStamp2 = nullptr;
	ID3D11Query * disJoint = nullptr;


	D3D11_QUERY_DESC qd = {};
	qd.Query = D3D11_QUERY_EVENT;

	device->CreateQuery(&qd, &async);

	qd.Query = D3D11_QUERY_TIMESTAMP;
	device->CreateQuery(&qd, &timeStamp);
	device->CreateQuery(&qd, &timeStamp2);

	qd.Query = D3D11_QUERY_TIMESTAMP_DISJOINT;
	device->CreateQuery(&qd, &disJoint);

	int plzDontBreak = 2;

	deviceContext->End(timeStamp2);
	UINT64 timestampStart;
	while (S_OK != deviceContext->GetData(timeStamp2, &timestampStart, sizeof(UINT64), 0))
	{
		plzDontBreak++;
	}
	
	DirectX::XMUINT4 DispatchOffset(0, 0, 0, 0);

	D3D11_BUFFER_DESC bDesc = {};
	bDesc.Usage = D3D11_USAGE_DYNAMIC;
	bDesc.ByteWidth = sizeof(DirectX::XMUINT4);
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bDesc.MiscFlags = 0;
	bDesc.StructureByteStride = 0;

	device->CreateBuffer(&bDesc, nullptr, &offsetBuffer);


#pragma region CREATE_GPU
	// Dunka upp qt till GPU
	{
		void * qtSer = malloc(TREE_SIZE);
		UINT currentOffset = 0;
		for (size_t i = 0; i < qt.size(); i++)
		{
			UINT sizeOfTriInd = (UINT)qt[i].TriangleIndices.size() * sizeof(UINT);
			memcpy((char*)qtSer + currentOffset, &qt[i], qt[i].ByteSize - sizeOfTriInd);
			currentOffset += qt[i].ByteSize - sizeOfTriInd;
			memcpy((char*)qtSer + currentOffset, qt[i].TriangleIndices.data(), sizeOfTriInd);
			currentOffset += sizeOfTriInd;
		}

		D3D11_BUFFER_DESC qtBufferDesc = {};
		qtBufferDesc.ByteWidth = currentOffset;
		qtBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		qtBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		qtBufferDesc.StructureByteStride = 0;
		qtBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		
		D3D11_BUFFEREX_SRV bsrv = {};
		bsrv.FirstElement = 0;
		bsrv.NumElements = currentOffset / 4;
		bsrv.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		srvDesc.BufferEx = bsrv;
		
		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = qtSer;
		subData.SysMemPitch = qtBufferDesc.ByteWidth;

		HRESULT hr = device->CreateBuffer(&qtBufferDesc, &subData, &quadTreeOld);
		hr = device->CreateShaderResourceView(quadTreeOld, &srvDesc, &quadTree);

		free(qtSer);
	}
	
	// Dunka upp trianglar till GPU
	{
		D3D11_BUFFER_DESC triBufferDesc = {};
		triBufferDesc.ByteWidth = bTri.size() * sizeof(Triangle);
		triBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		triBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		triBufferDesc.StructureByteStride = sizeof(Triangle);
		triBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_BUFFEREX_SRV bsrv = {};
		bsrv.FirstElement = 0;
		bsrv.NumElements = bTri.size();
		

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx = bsrv;

		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = bTri.data();
		subData.SysMemPitch = triBufferDesc.ByteWidth;

		HRESULT hr = device->CreateBuffer(&triBufferDesc, &subData, &trianglesOld);
		hr = device->CreateShaderResourceView(trianglesOld, &srvDesc, &triangles);
		
	}
	// Dunka upp WP till GPU
	{
		D3D11_BUFFER_DESC wpBufferDesc = {};
		wpBufferDesc.ByteWidth = gpuWp.size() * sizeof(gpuWaypoint);
		wpBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		wpBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		wpBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		wpBufferDesc.StructureByteStride = sizeof(gpuWaypoint);
		wpBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_BUFFER_UAV buav = {};
		buav.FirstElement = 0;
		buav.NumElements = gpuWp.size();
		buav.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer = buav;


		D3D11_SUBRESOURCE_DATA subData = {};
		subData.pSysMem = gpuWp.data();
		subData.SysMemPitch = wpBufferDesc.ByteWidth;

		HRESULT hr = device->CreateBuffer(&wpBufferDesc, &subData, &wpsOld);
		hr = device->CreateUnorderedAccessView(wpsOld, &uavDesc, &wps);
	}
#pragma endregion 

	ID3D11ComputeShader * cShader = nullptr;

#pragma region Create_Shader
	{
		HRESULT shaderError = 0;
		ID3DBlob * pCS = nullptr;
		ID3DBlob * er = nullptr;

		shaderError = D3DCompileFromFile(
			L"Rendering\\Rendering\\Shaders\\ComputeShaders\\ComputeShader.hlsl",
			nullptr,
			D3D_COMPILE_STANDARD_FILE_INCLUDE,
			"main",
			"cs_5_0",
			0,
			0,
			&pCS,
			&er
		);

		if (FAILED(shaderError))
		{
			std::cout << ((char*)er->GetBufferPointer());
			er->Release();
			if (pCS)
			{
				pCS->Release();
			}
		}
		HRESULT hr = device->CreateComputeShader(pCS->GetBufferPointer(), pCS->GetBufferSize(), nullptr, &cShader);
	}
	deviceContext->VSSetShader(nullptr, nullptr, NULL);
	deviceContext->PSSetShader(nullptr, nullptr, NULL);
	deviceContext->CSSetShader(cShader, nullptr, NULL);
#pragma endregion

#pragma region Bind_Buffer
	{
		deviceContext->CSSetShaderResources(0, 1, &triangles);
		deviceContext->CSSetShaderResources(1, 1, &quadTree);
		UINT i = -1;
		deviceContext->CSSetUnorderedAccessViews(0, 1, &wps, &i);
	}
#pragma endregion 

	Vertex v1, v2;
	DirectX::XMFLOAT4A n = { 0.0f, 1.0f, 0.0f, 0.0f };
	v1.Normal = n;
	v2.Normal = n;
	v1.Position.w = 1.0f;
	v2.Position.w = 1.0f;

	size_t increment = 512;

	size_t gpuwpSize = gpuWp.size() + increment - 1;

	int error = 0;

	double totalGpuTime = 0.0;

	std::cout << "\n";
	deviceContext->Begin(disJoint);
	for (size_t i = 0; i < gpuwpSize; i += increment)
	{
		error = 0;
		std::cout << "\r" << (double)i / gpuwpSize * 100.0 << " %               ";
		D3D11_MAPPED_SUBRESOURCE offsetData;

		deviceContext->Map(offsetBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &offsetData);

		memcpy(offsetData.pData, &DispatchOffset, sizeof(DirectX::XMUINT4));

		deviceContext->Unmap(offsetBuffer, 0);

		deviceContext->CSSetConstantBuffers(0, 1, &offsetBuffer);

		deviceContext->Dispatch(increment, 1, 1);
		
		DispatchOffset.x += increment;
		deviceContext->End(async);

		UINT queryData;
		
		while(S_OK != deviceContext->GetData(async, &queryData, sizeof(UINT), 0))
		{
			error++;
		}
	}

	std::cout << "\r" << 100.0 << " %               ";
	D3D11_MAPPED_SUBRESOURCE dataPtr;

	gpuWaypoint * arr = nullptr;
	if (SUCCEEDED(deviceContext->Map(wpsOld, 0, D3D11_MAP_READ, 0, &dataPtr)))
	{
		arr = (gpuWaypoint*)dataPtr.pData;

		for (size_t t = 0; t < gpuWp.size(); t++)
		{
			UINT nrOfConnections = arr[t].nrOfConnections;

			UINT key = arr[t].key;

			m_blockedTriangleTree.AddObject(&m_waypoints[key]);

			for (UINT n = 0; n < nrOfConnections && n < 256; n++)
			{
				UINT key2 = arr[t].connections[n];

				bool hasConnection = false;

				hasConnection = m_waypoints[key].Connect(&m_waypoints[key2]);

				if (m_waypoints[key2].Connect(&m_waypoints[key]))
				{
					if (hasConnection)
					{
						v1.Position.x = m_waypoints[key].GetPosition().x;
						v1.Position.y = m_waypoints[key].GetHeightVal();
						v1.Position.z = m_waypoints[key].GetPosition().y;
						v2.Position.x = m_waypoints[key2].GetPosition().x;
						v2.Position.y = m_waypoints[key2].GetHeightVal();
						v2.Position.z = m_waypoints[key2].GetPosition().y;
						m_connectionMesh.push_back(v1);
						m_connectionMesh.push_back(v2);
						counter++;
					}
				}
					
			}
		}
		deviceContext->Unmap(wpsOld, 0);
	}


	deviceContext->End(disJoint);
	D3D11_QUERY_DATA_TIMESTAMP_DISJOINT gpuTick;
	while (S_OK != deviceContext->GetData(disJoint, &gpuTick, sizeof(D3D11_QUERY_DATA_TIMESTAMP_DISJOINT), 0))
	{
		error++;
	}

	deviceContext->End(timeStamp);
	UINT64 timestampEnd;
	while (S_OK != deviceContext->GetData(timeStamp, &timestampEnd, sizeof(UINT64), 0))
	{
		error++;
	}

	double time = (timestampEnd - timestampStart) / gpuTick.Frequency;
	time *= 1000;

	deviceContext->CSSetShader(nullptr, nullptr, NULL);
	quadTreeOld->Release();
	wpsOld->Release();
	trianglesOld->Release();
	quadTree->Release();
	wps->Release();
	triangles->Release();
	cShader->Release();
	offsetBuffer->Release();
	async->Release();
	timeStamp->Release();
	disJoint->Release();
	timeStamp2->Release();

	std::cout << "\nConnection(s): " << counter << "... ";
	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
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

void Game::_creatingCoins()
{
	std::cout << "Placing Coins... ";
	Timer t;
	t.Start();
	int YetToPlace = m_numberOfCoins;
	m_coins = std::vector<Drawable>(m_numberOfCoins);
	m_numberOfCoins = 0;

	DirectX::XMFLOAT2 center(TERRAIN_SIZE * 0.5f, TERRAIN_SIZE * 0.5f);

	bool centerFix = false;
	while (!centerFix)
	{
		centerFix = true;
		Triangle * tri = m_blockedTriangleTree.PointInsideTriangle(center, true);
		if (tri)
		{
			centerFix = false;

			center.x += 1;
		}
	}

	Triangle * tri = m_unblockedTriangleTree.PointInsideTriangle(center, true);
	m_player.SetPosition(center.x, tri->points[0].y, center.y);

	size_t unblockedSize = (m_unblockedTriangles.size() - 1);

	while (YetToPlace > 0)
	{
		float proc = ((float)rand() / (float)RAND_MAX);

		size_t triIndex = unblockedSize * proc;

		DirectX::XMFLOAT3 spawn(0,0,0);

		for (int i = 0; i < 3; i++)
		{
			spawn.x += m_unblockedTriangles[triIndex]->points[i].x;
			spawn.y += m_unblockedTriangles[triIndex]->points[i].y;
			spawn.z += m_unblockedTriangles[triIndex]->points[i].z;
		}
		spawn.x /= 3.0f;
		spawn.y /= 3.0f;
		spawn.z /= 3.0f;

		std::vector<DirectX::XMFLOAT2> p;
		if (!(p = Pathfinding::FindPath(m_player.GetPosition(), spawn, m_blockedTriangleTree)).empty())
		{
			spawn.x = p.back().x;
			spawn.y = -500;
			spawn.z = p.back().y;
			DirectX::XMFLOAT3 iPoint;
			Triangle * t = m_unblockedTriangleTree.RayIntersectionTriangle3D(spawn, DirectX::XMFLOAT3(0,1,0), true, iPoint);
			if (t == nullptr)
				std::cout << "THIS IS BAD AF!\n\n";
			spawn.y = iPoint.y + 2.5f;
			m_coins[m_numberOfCoins].SetVertices(&m_playerMesh);
			m_coins[m_numberOfCoins].SetColor(1, 1, 0);
			m_coins[m_numberOfCoins].SetPosition(spawn);
			m_coins[m_numberOfCoins].SetScale(5, 5, 5);
			m_coins[m_numberOfCoins].Update();
			m_unblockedTriangleTree.AddObject(&m_coins[m_numberOfCoins]);
			m_numberOfCoins++;
			YetToPlace--;
		}
	}

	

	std::cout << t.Stop(Timer::MILLISECONDS) << " ms\n";
}
