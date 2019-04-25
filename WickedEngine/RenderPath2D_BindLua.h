#pragma once
#include "wiLua.h"
#include "wiLuna.h"
#include "RenderPath2D.h"
#include "RenderPath_BindLua.h"

class RenderPath2D_BindLua : public RenderPath_BindLua
{
public:
	static const char className[];
	static Luna<RenderPath2D_BindLua>::FunctionType methods[];
	static Luna<RenderPath2D_BindLua>::PropertyType properties[];

	RenderPath2D_BindLua(RenderPath2D* component = nullptr);
	RenderPath2D_BindLua(lua_State *L);
	~RenderPath2D_BindLua();

	static void Bind();
};

