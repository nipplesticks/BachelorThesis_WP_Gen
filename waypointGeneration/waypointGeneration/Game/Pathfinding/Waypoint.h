#pragma once
#include "../../Rendering/Drawable/Drawable.h"

class Waypoint
{
public:
	Waypoint(float x, float y);
	~Waypoint();

	void SetPosition(float x, float y);
	const DirectX::XMFLOAT2 GetPosition();

	bool operator==(const Waypoint & other);
	bool operator<(const Waypoint & other);

private:
	/*
	
		Stuff

	*/
	DirectX::XMFLOAT2 m_position;

};