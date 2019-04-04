#pragma once
#include "Quadrant.h"

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
	void PlaceObjects(std::vector<Drawable*> & objectVector);

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
	size_t _GetQuadrantIndex(const sf::Vector2f & worldPos, unsigned int level);
	void _traverseAndPlace(Entity * e, int quadIndex);
	void _pointTraverse(const sf::Vector2f & point, int quadIndex, Entity *& ePtr) const;
	void _traverseWithRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, int quadIndex, float & t, Entity *& ePtr) const;
	bool _lineWithLineIntersection(const sf::Vector2f & lineOrigin1, const sf::Vector2f & lineEnd1, const sf::Vector2f & lineOrigin2, const sf::Vector2f & lineEnd2, float & t) const;
	bool _insideRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, const Quadrant & quadrant) const;
	bool _insideRay(const sf::Vector2f & rayStart, const sf::Vector2f & rayEnd, const Entity * e, float & t) const;
	bool _insideAABB(const sf::Vector2f & min, const sf::Vector2f & size, const Quadrant & quadrant) const;
	bool _insideAABB(const sf::Vector2f & min, const Quadrant & quadrant) const;
};

