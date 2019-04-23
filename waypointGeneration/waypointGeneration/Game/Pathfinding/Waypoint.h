#pragma once
#include <map>

class Waypoint
{
public:
	struct WaypointConnection
	{
		Waypoint * wp = nullptr;
		float connectionCost = FLT_MAX;
	};

	Waypoint(float x = 0.0f, float y = 0.0f);
	~Waypoint();

	void SetHeightVal(float height);
	float GetHeightVal() const;
	void SetPosition(float x, float y);
	const DirectX::XMFLOAT2 GetPosition();

	bool Connect(Waypoint * wp);
	void ForceConnection(Waypoint * wp);

	bool operator==(const Waypoint & other);
	bool operator<(const Waypoint & other);

private:	

	std::map<int, WaypointConnection> m_connections;

	DirectX::XMFLOAT2 m_position;
	float m_heightValue = 0;

	

};