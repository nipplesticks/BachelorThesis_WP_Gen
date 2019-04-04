#pragma once
#include "../Rendering/Drawable/Drawable.h"
#include "../Rendering/Camera/Camera.h"
#include "../MapGeneration/TerrainCreator.h"
#include "../MapGeneration/DiamondSquare.h"

#define MOUSE_SESITIVITY_X	0.05
#define MOUSE_SESITIVITY_Y	0.05
#define CAMERA_XZ_SPEED		100.0
#define CAMERA_Y_SPEED		100.0

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
	std::vector<Vertex> m_buildingMeshBox;
	std::vector<Vertex> m_buildingMeshCylinder;

	DiamondSquare m_diamondSquare;
	TerrainCreator m_terrainCreator;
	Drawable m_terrain;
	Drawable m_player;

	POINT m_mousePosLastFrame;

	std::vector<Drawable> m_buildings;
private:
	void _playerFixYPosition();
	void _cameraControl(double dt);

private:
	void _loadTerrain();
	void _loadMeshes();
	void _randomizeBuildings();
};