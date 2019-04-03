#include "waypointGenerationPCH.h"
#include "IRender.h"

std::vector<Drawable*> IRender::p_drawQueue;


IRender::IRender()
{
}

IRender::~IRender()
{
}

void IRender::Queue(Drawable * drawable)
{
	p_drawQueue.push_back(drawable);
}

void IRender::Clear()
{
	p_drawQueue.clear();
}
