#pragma once
#include "GLApplication.h"
class TerrainApp :
	public GLApplication
{
public:
	TerrainApp(int width, int height);
	~TerrainApp();
protected:
	void OnKey(int key);
	void RenderLoop();
};

