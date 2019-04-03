#pragma once
#include "../../Drawable/Drawable.h"

class IRender
{
public:
	IRender();
	virtual ~IRender();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	static void Queue(Drawable * drawable);
	static void Clear();

private:
	static std::vector<Drawable*> m_drawQueue;
};