#include "waypointGenerationPCH.h"
#include "IRender.h"

std::vector<Drawable*> IRender::m_drawQueue;


IRender::IRender()
{
}

IRender::~IRender()
{
}

void IRender::Queue(Drawable * drawable)
{
	m_drawQueue.push_back(drawable);
}

void IRender::Clear()
{
	m_drawQueue.clear();
}
