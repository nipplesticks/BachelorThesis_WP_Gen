#pragma once
#include "Quadrant.h"
//
class QuadTree
{
public:
	QuadTree();
	~QuadTree();

	void BuildTree(unsigned int maxLevel, unsigned int worldSize, const DirectX::XMFLOAT2 & worldStart);
	void BuildTree(unsigned int maxLevel, unsigned int worldSize, float worldStartX, float worldStartY);

	void PlaceObjects(std::vector<Drawable*> & objectVector);
	void PlaceObjects(std::vector<Waypoint*> & objectVector);
	void PlaceObjects(std::vector<Triangle*> & objectVector);

	unsigned int GetWorldSize() const;
	unsigned int GetMaxTreeLevel() const;

	Drawable * LineIntersection(const DirectX::XMFLOAT2 & origin, const DirectX::XMFLOAT2 & direction, float & t, Drawable * avoidThis);
	Triangle * PointIntersection(const DirectX::XMFLOAT2 & point, Drawable * avoidThis);

private:
	std::vector<Quadrant> m_quadTree;
	std::vector<Quadrant*> m_leafs;
	unsigned int m_worldSize;
	unsigned int m_maximumLevel;

private:
	size_t _GetQuadrantIndex(const DirectX::XMFLOAT2 & worldPos, unsigned int level);
	void _traverseAndPlace(Drawable * e, int quadIndex);
	void _traverseAndPlace(Waypoint * e, int quadIndex);
	void _traverseAndPlace(Triangle * e, int quadIndex);
	void _pointTraverse(const DirectX::XMFLOAT2 & point, int quadIndex, Drawable *& ePtr) const;
	void _traverseWithRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, int quadIndex, float & t, Drawable *& ePtr) const;
	bool _lineWithLineIntersection(const DirectX::XMFLOAT2 & lineOrigin1, const DirectX::XMFLOAT2 & lineEnd1, const DirectX::XMFLOAT2 & lineOrigin2, const DirectX::XMFLOAT2 & lineEnd2, float & t) const;
	bool _insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Quadrant & quadrant) const;
	bool _insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Drawable * e, float & t) const;
	bool _insideAABB(const DirectX::XMFLOAT2 & min, const DirectX::XMFLOAT2 & size, const Quadrant & quadrant) const;
	bool _insideAABB(const DirectX::XMFLOAT2 & min, const Quadrant & quadrant) const;
};

