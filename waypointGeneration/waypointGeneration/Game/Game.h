#pragma once
#include <map>
#include "../Rendering/Drawable/Drawable.h"
#include "../Rendering/Camera/Camera.h"
#include "../MapGeneration/TerrainCreator.h"
#include "../MapGeneration/DiamondSquare.h"
#include "Pathfinding/Waypoint.h"
#include "QuadTree/QuadTree.h"

#define DRAW_WAYPOINT true
#define DRAW_TRIANGLES false
#define DRAW_CONNECTIONS false
#define USE_RANDOM_SEED false

#define MIN_HEIGHT	-10
#define MAX_HEIGHT	15
#define NOISE_MIN	15
#define NOISE_RAND	6

#define MOUSE_SESITIVITY_X	0.05
#define MOUSE_SESITIVITY_Y	0.05
#define CAMERA_XZ_SPEED		TERRAIN_SIZE / 10
#define CAMERA_ZOOM_SPEED	10.0

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

	QuadTree m_blockedTriangleTree;
	QuadTree m_unblockedTriangleTree;

	std::vector<Triangle*> m_blockedTriangles;
	std::vector<Vertex> m_blockedTrianglesVertices;
	Drawable m_blockedtrianglesDraw;

	std::vector<Triangle*> m_unblockedTriangles;
	std::vector<Vertex> m_unblockedTrianglesVertices;
	Drawable m_unblockedtrianglesDraw;

	std::vector<Vertex> m_connectionMesh;
	Drawable m_connections;

	//std::vector<Drawable> m_wp;
	std::map<Waypoint*, Drawable> m_wpDraw;
	Waypoint * m_target = nullptr;

	std::vector<Vertex> m_edgeMeshes[4];
	Drawable m_edges[4];

	ID3D11Texture2D *			m_terrainTex2D = nullptr;
	ID3D11ShaderResourceView *	m_terrainTexture = nullptr;

	ID3D11ShaderResourceView * m_waterTexture = nullptr;
	ID3D11Texture2D * m_waterTex2D = nullptr;

	POINT m_mouseReferencePosition;

	bool m_isFollowingPlayer = false;
	float m_distanceToPlayer;

	float m_maxHeight = 0.0f;
	float m_minHeight = 0.0f;
	float m_noise = 0.0f;

	std::map<long int, Waypoint> m_waypoints;

	std::vector<Drawable> m_buildings;
private:
	void _playerFixYPosition(double dt);
	void _cameraControl(double dt);

private:
	void _createWaterTexture();
	void _createWorld();
	void _loadMeshes();
	void _randomizeBuildings();
	void _setupGame();

private:
	void _createTerrain();
	void _buildTrees();
	void _createBlockedTriangles();
	void _createWaypoints();
	void _cleanWaypoints();
	void _placeTrianglesInTree();
	void _smoothUnorthodoxShapes();
	void _createViewableTriangles();
	void _createViewableWaypoints();
	void _offsetWaypoints();
	void _connectWaypoints();
	void _createViewableConnections();
	void _generateWorldEdges();
};