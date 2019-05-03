#include "waypointGenerationPCH.h"
#include "Waypoint.h"

#include <DirectXMath.h>
Waypoint::Waypoint(float x, float y)
{
	SetPosition(x, y);
}

Waypoint::~Waypoint()
{
}

void Waypoint::SetHeightVal(float height)
{
	m_heightValue = height;
}

float Waypoint::GetHeightVal() const
{
	return m_heightValue;
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

int Waypoint::Connect(Waypoint * wp)
{
	if (wp != this)
	{
		int index = wp->GetPosition().x + wp->GetPosition().y * TERRAIN_SIZE;

		auto it = m_connections.find(index);

		if (it == m_connections.end())
		{
			float l = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&wp->GetPosition()), DirectX::XMLoadFloat2(&m_position))));
			DirectX::XMVECTOR vDir = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&wp->GetPosition()), DirectX::XMLoadFloat2(&m_position)));
			

			// is 0 if it should not be added
			// is 1 if it should be added
			// is index if it should be swapped

			int canAdd = 1;
			for (auto & wc : m_connections)
			{
				DirectX::XMVECTOR vDir2 = DirectX::XMVector2Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&wc.second.wp->GetPosition()), DirectX::XMLoadFloat2(&m_position)));
				float dot = DirectX::XMVectorGetX(DirectX::XMVector2Dot(vDir, vDir2));

				if (dot > 0.95f)
				{
					float oldLength = wc.second.connectionCost;
					if (l < oldLength)
					{
						canAdd = wc.first;
					}
					else
					{
						canAdd = 0;
					}
					break;
				}
			}

			if (canAdd > 0)
			{
				int add = 1;
				if (canAdd != 1)
				{
					m_connections.erase(canAdd);
					add = 0;
				}
				
				WaypointConnection wc = {};
				wc.wp = wp;
				wc.connectionCost = l;
				m_connections.insert(std::make_pair(index, wc));

				return add;
			}
		}
	}

	return -1;
}

void Waypoint::ForceConnection(Waypoint * wp)
{
	if (wp != this)
	{
		int index = wp->GetPosition().x + wp->GetPosition().y * TERRAIN_SIZE;

		auto it = m_connections.find(index);

		if (it == m_connections.end())
		{
			WaypointConnection wc;
			wc.wp = wp;

			wc.connectionCost = DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(DirectX::XMLoadFloat2(&wp->GetPosition()), DirectX::XMLoadFloat2(&m_position))));
			m_connections.insert(std::make_pair(index, wc));
		}
	}
}

std::map<int, Waypoint::WaypointConnection>* Waypoint::GetConnections()
{
	return &m_connections;
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
