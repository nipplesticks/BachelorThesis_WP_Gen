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

	void AddObject(Triangle * triangle);
	void AddObject(Waypoint * wp);

	void PlaceObjects(std::vector<Drawable*> & objectVector);
	void PlaceObjects(std::vector<Waypoint*> & objectVector);
	void PlaceObjects(std::vector<Triangle*> & objectVector);

	unsigned int GetWorldSize() const;
	unsigned int GetMaxTreeLevel() const;

	Triangle * LineIntersectionTriangle(const DirectX::XMFLOAT2 & lineStart, const DirectX::XMFLOAT2 & lineEnd,
		bool firstHitFound, __out DirectX::XMFLOAT2 & interSectionPoint);
	Triangle * RayIntersectionTriangle3D(const DirectX::XMFLOAT3 & rayOrigin, const DirectX::XMFLOAT3 & rayDir,
		bool firstHitFound, __out DirectX::XMFLOAT3 & interSectionPoint);
	Triangle * PointInsideTriangle(const DirectX::XMFLOAT2 & point,	bool firstHitFound);

	Waypoint * FindClosestWaypoint(const DirectX::XMFLOAT3 & position, float radius);

	//Drawable * LineIntersection(const DirectX::XMFLOAT2 & origin, const DirectX::XMFLOAT2 & direction, float & t, Drawable * avoidThis);
	//Triangle * PointIntersection(const DirectX::XMFLOAT2 & point, Drawable * avoidThis);

private:
	std::vector<Quadrant> m_quadTree;
	std::vector<Quadrant*> m_leafs;
	unsigned int m_worldSize;
	unsigned int m_maximumLevel;

private:
	void _triangleTraversalLine(__out Triangle *& outPtr, __out DirectX::XMFLOAT2 & interSectionPoint,
		const DirectX::XMVECTOR & lineStart, const DirectX::XMVECTOR & lineEnd, const DirectX::XMVECTOR & dir, bool firstHitFound, int quadIndex, float & t);
	void _triangleTraversalRay3D(__out Triangle *& outPtr, __out DirectX::XMFLOAT3 & interSectionPoint,
		const DirectX::XMVECTOR & rayOrigin, const DirectX::XMVECTOR & rayOrigin2D,
		const DirectX::XMVECTOR & ray, const DirectX::XMVECTOR & ray2D,
		bool firstHitFound, int quadIndex, float & t);
	void _triangleTraversePoint(__out Triangle *& tPtr, const DirectX::XMVECTOR & p, int quadIndex);

	void _closestWaypoint(Waypoint *& wp, float & dist, DirectX::XMVECTOR pos, const DirectX::BoundingSphere & bs, UINT quadIndex);

	bool _lineTriangleIntersection(const Triangle * tri, __out DirectX::XMFLOAT2 & interSectionPoint,
		const DirectX::XMVECTOR & lineStart, const DirectX::XMVECTOR & lineEnd, float & t);
	bool _Ray3DTriangleIntersection(const Triangle * tri,
		const DirectX::XMVECTOR & origin, const DirectX::XMVECTOR & dir, float & t);
	bool _PointTriangleIntersection(const Triangle * tri, const DirectX::XMVECTOR & p);

private:
	size_t _GetQuadrantIndex(const DirectX::XMFLOAT2 & worldPos, unsigned int level);
	void _traverseAndPlace(Drawable * e, int quadIndex);
	void _traverseAndPlace(Waypoint * e, int quadIndex);
	void _traverseAndPlace(Triangle * e, int quadIndex);
	void _pointTraverse(const DirectX::XMFLOAT2 & point, int quadIndex, Drawable *& ePtr) const;
	void _traverseWithRay(const DirectX::XMFLOAT2 & rayOrigin, const DirectX::XMFLOAT2 & rayDirection, int quadIndex, float & t, Drawable *& ePtr, Drawable * avoidThis) const;
	bool _lineWithLineIntersection(const DirectX::XMFLOAT2 & lineOrigin1, const DirectX::XMFLOAT2 & lineEnd1, const DirectX::XMFLOAT2 & lineOrigin2, const DirectX::XMFLOAT2 & lineEnd2, float & t) const;
	bool _insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Quadrant & quadrant) const;
	bool _insideRay(const DirectX::XMFLOAT2 & rayStart, const DirectX::XMFLOAT2 & rayEnd, const Drawable * e, float & t) const;
	bool _insideAABB(const DirectX::XMFLOAT2 & min, const DirectX::XMFLOAT2 & size, const Quadrant & quadrant) const;
	bool _insideAABB(const DirectX::XMFLOAT2 & min, const Quadrant & quadrant) const;
};

