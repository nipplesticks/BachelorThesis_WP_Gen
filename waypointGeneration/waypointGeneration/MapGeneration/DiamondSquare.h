#pragma once

class DiamondSquare
{
public:
	DiamondSquare();
	~DiamondSquare();

	std::vector<float> CreateDiamondSquare(int mapSize, int stepSize, float noise);

private:
	int m_mapSize;
	std::vector<float> m_diamondSquare;

	float _fRand(int min, int max);
	float _getValue(int x, int z);
	void _setValue(int x, int z, float value);
	void _diamondStep(int x, int z, int stepSize, int halfStep, float noise);
	void _squareStep(int x, int z, int stepSize, int halfStep, float noise);
	void _diamondSquareAlgorithm(int stepSize, float noise);

	// Smoothing
	void _smoothValues(int filtersize); // filtersize must be 2^n + 1
};