#include "waypointGenerationPCH.h"
#include "Waypoint.h"

Waypoint::Waypoint(float x, float y)
{
	SetPosition(x, y);
}

Waypoint::~Waypoint()
{
}

void Waypoint::SetPosition(float x, float y)
{
	m_position.x = x;
	m_position.y = y;
}

const DirectX::XMFLOAT2 Waypoint::GetPosition()
{
	return m_position;
}

bool floatEQ(float a, float b)
{
	return fabs(a - b) < 0.01f;
}

bool Waypoint::operator==(const Waypoint & other)
{
	return floatEQ(m_position.x, other.m_position.x) && floatEQ(m_position.y, other.m_position.y);
}

bool Waypoint::operator<(const Waypoint & other)
{
	bool xValLess = m_position.x < other.m_position.x;
	bool yValLess = m_position.y < other.m_position.y;

	return yValLess || (fabs(m_position.y - other.m_position.y) < 0.01f && xValLess);
}
