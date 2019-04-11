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
	virtual void Release() = 0;

	static void Queue(Drawable * drawable);
	static void QueueAlpha(Drawable * drawable);
	static void QueueNoDepth(Drawable * drawable);
	static void Clear();

protected:
	static std::vector<Drawable*> p_drawQueue;
	static std::vector<Drawable*> p_drawQueueTransparent;
	static std::vector<Drawable*> p_drawQueueNoDepthBuffer;
};