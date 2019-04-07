#include "waypointGenerationPCH.h"
#include "IRender.h"

std::vector<Drawable*> IRender::p_drawQueue;
std::vector<Drawable*> IRender::p_drawQueueTransparent;


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

void IRender::QueueAlpha(Drawable * drawable)
{
	p_drawQueueTransparent.push_back(drawable);
}


void IRender::Clear()
{
	p_drawQueueTransparent.clear();
	p_drawQueue.clear();
}
