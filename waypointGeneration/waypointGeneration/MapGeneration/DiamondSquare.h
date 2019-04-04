#pragma once

class DiamondSquare
{
public:
	DiamondSquare();
	~DiamondSquare();

	std::vector<float> CreateDiamondSquare(int mapSize, int stepSize, float noise);
	// Requires unified arraysize of all the arrays
	std::vector<std::vector<float>> AdvancedCreateDiamondSquare(int mapSize[], int stepSize[], float noise[], int arraySize);

private:
	int m_mapSize;
	std::vector<float> m_diamondSquare;

	float _fRand();
	float _getValue(int x, int z);
	void _setValue(int x, int z, float value);
	void _diamondStep(int x, int z, int stepSize, int halfStep, float noise);
	void _squareStep(int x, int z, int stepSize, int halfStep, float noise);
	void _diamondSquareAlgorithm(int stepSize, float noise);

	// Advanced diamond square algorithm creating different parts of the map with different values.
	// The algorithm will create separate parts and then connect them.
	// This makes it possible to have different map widths in the same map.
	float _advancedGetValue(const std::vector<float> & map, int mapSize, int x, int z);
	void _advancedSetValue(std::vector<float> & map, int mapSize, int x, int z, float value);
	void _advancedDiamondStep(std::vector<float> & map, int mapSize, int x, int z, int stepSize, int halfStep, float noise);
	void _advancedSquareStep(std::vector<float> & map, int mapSize, int x, int z, int stepSize, int halfStep, float noise);
	void _advancedDiamondSquareAlgorithm(std::vector<std::vector<float>> & map, int mapSize, int stepSize, float noise);

	// Smoothing
	void _smoothValues(int filtersize); // filtersize must be 2^n + 1
	void _advancedSmoothValues(std::vector<float> & map, int filtersize);
};