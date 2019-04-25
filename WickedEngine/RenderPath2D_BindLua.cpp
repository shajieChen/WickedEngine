#include "RenderPath2D_BindLua.h"
#include "wiResourceManager_BindLua.h"
#include "wiSprite_BindLua.h"
#include "wiFont_BindLua.h"

#include <sstream>

using namespace std;

const char RenderPath2D_BindLua::className[] = "RenderPath2D";

Luna<RenderPath2D_BindLua>::FunctionType RenderPath2D_BindLua::methods[] = {
	lunamethod(RenderPath2D_BindLua, GetContent),
	lunamethod(RenderPath2D_BindLua, Initialize),
	lunamethod(RenderPath2D_BindLua, Load),
	lunamethod(RenderPath2D_BindLua, Unload),
	lunamethod(RenderPath2D_BindLua, Start),
	lunamethod(RenderPath2D_BindLua, Stop),
	lunamethod(RenderPath2D_BindLua, FixedUpdate),
	lunamethod(RenderPath2D_BindLua, Update),
	lunamethod(RenderPath2D_BindLua, Render),
	lunamethod(RenderPath2D_BindLua, Compose),
	lunamethod(RenderPath_BindLua, OnStart),
	lunamethod(RenderPath_BindLua, OnStop),
	lunamethod(RenderPath_BindLua, GetLayerMask),
	lunamethod(RenderPath_BindLua, SetLayerMask),
	{ NULL, NULL }
};
Luna<RenderPath2D_BindLua>::PropertyType RenderPath2D_BindLua::properties[] = {
	{ NULL, NULL }
};

RenderPath2D_BindLua::RenderPath2D_BindLua(RenderPath2D* component)
{
	this->component = component;
}

RenderPath2D_BindLua::RenderPath2D_BindLua(lua_State *L)
{
	component = new RenderPath2D();
}


RenderPath2D_BindLua::~RenderPath2D_BindLua()
{
}

void RenderPath2D_BindLua::Bind()
{
	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		Luna<RenderPath2D_BindLua>::Register(wiLua::GetGlobal()->GetLuaState());
	}
}