#pragma once
#include <map>
#include "../Rendering/Drawable/Drawable.h"
#include "../Rendering/Camera/Camera.h"
#include "../MapGeneration/TerrainCreator.h"
#include "../MapGeneration/DiamondSquare.h"
#include "Pathfinding/Waypoint.h"

#define MOUSE_SESITIVITY_X	0.05
#define MOUSE_SESITIVITY_Y	0.05
#define CAMERA_XZ_SPEED		100.0
#define CAMERA_ZOOM_SPEED	10.0
#define TERRAIN_SIZE		1025
//#define TERRAIN_SIZE		65

class Game
{
public:
	Game();
	~Game();

	void Update(double dt);
	void Draw();

private:
	Camera m_camera;

	std::vector<Vertex> m_playerMesh;
	std::vector<Vertex> m_terrainMesh;
	std::vector<Vertex> m_XZPlane;
	std::vector<Vertex> m_buildingMeshBox;
	std::vector<Vertex> m_buildingMeshCylinder;

	DiamondSquare m_diamondSquare;
	TerrainCreator m_terrainCreator;
	Drawable m_terrain;
	Drawable m_player;
	Drawable m_water;

	std::vector<Waypoint> m_wp;

	std::vector<Vertex> m_edgeMeshes[4];
	Drawable m_edges[4];

	ID3D11Texture2D *			m_terrainTex2D = nullptr;
	ID3D11ShaderResourceView *	m_terrainTexture = nullptr;

	ID3D11ShaderResourceView * m_waterTexture = nullptr;
	ID3D11Texture2D * m_waterTex2D = nullptr;

	POINT m_mouseReferencePosition;

	float m_maxHeight = 0.0f;
	float m_minHeight = 0.0f;

	std::map<long int, Waypoint> m_waypoints;

	std::vector<Drawable> m_buildings;
private:
	void _playerFixYPosition();
	void _cameraControl(double dt);

private:
	void _createWaterTexture();
	void _loadTerrain();
	void _loadMeshes();
	void _randomizeBuildings();
	void _setupGame();
};