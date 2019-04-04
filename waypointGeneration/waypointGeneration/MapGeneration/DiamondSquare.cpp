#include "waypointGenerationPCH.h"
#include "DiamondSquare.h"

DiamondSquare::DiamondSquare()
{
}

DiamondSquare::~DiamondSquare()
{
}

std::vector<float> DiamondSquare::CreateDiamondSquare(int mapSize, int stepSize, float noise)
{
	m_mapSize = mapSize;
	m_diamondSquare.resize(m_mapSize * m_mapSize);
	
	for (int z = 0; z < m_mapSize; z += stepSize)
		for (int x = 0; x < m_mapSize; x += stepSize)
			_setValue(x, z, _fRand());

	while (stepSize > 1)
	{
		_diamondSquareAlgorithm(stepSize, noise);

		stepSize = stepSize * 0.5;
		noise = noise * 0.5;
	}

	_smoothValues((int)pow(2, 2) + 1);

	return m_diamondSquare;
}

std::vector< std::vector<float>> DiamondSquare::AdvancedCreateDiamondSquare(int mapSize[], int stepSize[], float noise[], int arraySize)
{
	int totalMapSize = 0;
	for (int i = 0; i < arraySize; i++)
		totalMapSize += mapSize[i] * mapSize[i];
	m_mapSize = totalMapSize;
	std::vector<std::vector<float>> map;

	for (int i = 0; i < arraySize; i++)
	{
		while (stepSize[i] > 1)
		{
			_advancedDiamondSquareAlgorithm(map, mapSize[i], stepSize[i], noise[i]);

			stepSize[i] = stepSize[i] * 0.5;
			noise[i] = noise[i] * 0.5;
		}
	}

	return map;
}

float DiamondSquare::_fRand()
{
	int min = -15;
	int max = 15;
	float randomNumber = (float)rand() / RAND_MAX;

	return (min + randomNumber * (max - min));
}

float DiamondSquare::_getValue(int x, int z)
{
	if (x < m_mapSize && x >= 0)
		if (z < m_mapSize && z >= 0)
			return m_diamondSquare[x + (z * m_mapSize)];
	return 0.0f;
}

void DiamondSquare::_setValue(int x, int z, float value)
{
	m_diamondSquare[x + (z * m_mapSize)] = value;
}

void DiamondSquare::_diamondStep(int x, int z, int stepSize, int halfStep, float noise)
{
	float corner1 = _getValue(x - halfStep, z - halfStep);
	float corner2 = _getValue(x + halfStep, z - halfStep);
	float corner3 = _getValue(x - halfStep, z + halfStep);
	float corner4 = _getValue(x + halfStep, z + halfStep);
	float squareValue = ((corner1 + corner2 + corner3 + corner4) * 0.25) + noise;

	_setValue(x, z, squareValue);
}

void DiamondSquare::_squareStep(int x, int z, int stepSize, int halfStep, float noise)
{
	float corner1 = _getValue(x - halfStep, z);
	float corner2 = _getValue(x + halfStep, z);
	float corner3 = _getValue(x, z + halfStep);
	float corner4 = _getValue(x, z + halfStep);
	float diamondValue = ((corner1 + corner2 + corner3 + corner4) * 0.25) + noise;

	_setValue(x, z, diamondValue);
}

void DiamondSquare::_diamondSquareAlgorithm(int stepSize, float noise)
{
	int halfStep = stepSize * 0.5;

	for (int z = halfStep; z < m_mapSize - 1; z += stepSize)
		for (int x = halfStep; x < m_mapSize - 1; x += stepSize)
			_diamondStep(x, z, stepSize, halfStep, _fRand() * noise);

	for (int z = 0; z < m_mapSize - 1; z += stepSize)
		for (int x = 0; x < m_mapSize - 1; x += stepSize)
		{
			_squareStep(x + halfStep, z, stepSize, halfStep, _fRand() * noise);
			_squareStep(x + halfStep, z, stepSize, halfStep, _fRand() * noise);
		}
}

float DiamondSquare::_advancedGetValue(const std::vector<float> & map, int mapSize, int x, int z)
{
	if (x < mapSize && x >= 0)
		if (z < mapSize && z >= 0)
			return map[x + (z * mapSize)];
}

void DiamondSquare::_advancedSetValue(std::vector<float> & map, int mapSize, int x, int z, float value)
{
	map[x + (z * mapSize)] = value;
}

void DiamondSquare::_advancedDiamondStep(std::vector<float> & map, int mapSize, int x, int z, int stepSize, int halfStep, float noise)
{
	float corner1 = _advancedGetValue(map, mapSize, x - halfStep, z - halfStep);
	float corner2 = _advancedGetValue(map, mapSize, x + halfStep, z - halfStep);
	float corner3 = _advancedGetValue(map, mapSize, x - halfStep, z + halfStep);
	float corner4 = _advancedGetValue(map, mapSize, x + halfStep, z + halfStep);
	float squareValue = ((corner1 + corner2 + corner3 + corner4) * 0.25) + noise;

	_advancedSetValue(map, mapSize, x, z, squareValue);
}

void DiamondSquare::_advancedSquareStep(std::vector<float> & map, int mapSize, int x, int z, int stepSize, int halfStep, float noise)
{
	float corner1 = _advancedGetValue(map, mapSize, x - halfStep, z);
	float corner2 = _advancedGetValue(map, mapSize, x + halfStep, z);
	float corner3 = _advancedGetValue(map, mapSize, x, z + halfStep);
	float corner4 = _advancedGetValue(map, mapSize, x, z + halfStep);
	float diamondValue = ((corner1 + corner2 + corner3 + corner4) * 0.25) + noise;

	_advancedSetValue(map, mapSize, x, z, diamondValue);
}

void DiamondSquare::_advancedDiamondSquareAlgorithm(std::vector<std::vector<float>> & map, int mapSize, int stepSize, float noise)
{
	int halfStep = stepSize * 0.5;
	std::vector<float> mapFractions;
	mapFractions.resize(mapSize * mapSize);

	for (int z = halfStep; z < mapSize - 1; z += stepSize)
		for (int x = halfStep; x < mapSize - 1; x += stepSize)
			_advancedDiamondStep(mapFractions, mapSize, x, z, stepSize, halfStep, _fRand() * noise);

	for (int z = 0; z < mapSize - 1; z += stepSize)
		for (int x = 0; x < mapSize - 1; x += stepSize)
		{
			_advancedSquareStep(mapFractions, mapSize, x + halfStep, z, stepSize, halfStep, _fRand() * noise);
			_advancedSquareStep(mapFractions, mapSize, x + halfStep, z, stepSize, halfStep, _fRand() * noise);
		}

	_advancedSmoothValues(mapFractions, (int)pow(2, 2) + 1);
	map.push_back(mapFractions);
}

void DiamondSquare::_smoothValues(int filtersize)
{
	int count = 0;
	float total = 0.0f;

	for (int x = 0; x < m_mapSize; x++)
	{
		for (int z = 0; z < m_mapSize; z++)
		{
			count = 0;
			total = 0.0f;

			for (int x0 = x - filtersize; x0 <= x + filtersize; x0++)
			{
				if (x0 < 0 || x0 > m_mapSize - 1)
					continue;

				for (int z0 = z - filtersize; z0 < z + filtersize; z0++)
				{
					if (z0 < 0 || z0 > m_mapSize - 1)
						continue;

						total += m_diamondSquare[z0 + (x0 * m_mapSize)];
						count++;
				}
			}
			if (count != 0 && total != 0)
				m_diamondSquare[z + (m_mapSize * x)] = total / (float)count;
		}
	}
}

void DiamondSquare::_advancedSmoothValues(std::vector<float>& map, int filtersize)
{
}
