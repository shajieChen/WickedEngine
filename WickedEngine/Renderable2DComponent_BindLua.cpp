#include "Renderable2DComponent_BindLua.h"
#include "wiResourceManager_BindLua.h"
#include "wiSprite_BindLua.h"
#include "wiFont_BindLua.h"

const char Renderable2DComponent_BindLua::className[] = "Renderable2DComponent";

Luna<Renderable2DComponent_BindLua>::FunctionType Renderable2DComponent_BindLua::methods[] = {
	lunamethod(Renderable2DComponent_BindLua, GetContent),
	lunamethod(Renderable2DComponent_BindLua, AddSprite),
	lunamethod(Renderable2DComponent_BindLua, AddFont),
	lunamethod(Renderable2DComponent_BindLua, RemoveSprite),
	lunamethod(Renderable2DComponent_BindLua, RemoveFont),
	lunamethod(Renderable2DComponent_BindLua, ClearSprites),
	lunamethod(Renderable2DComponent_BindLua, ClearFonts),
	lunamethod(Renderable2DComponent_BindLua, AddFont),
	lunamethod(Renderable2DComponent_BindLua, Initialize),
	lunamethod(Renderable2DComponent_BindLua, Load),
	lunamethod(Renderable2DComponent_BindLua, Unload),
	lunamethod(Renderable2DComponent_BindLua, Start),
	lunamethod(Renderable2DComponent_BindLua, Stop),
	lunamethod(Renderable2DComponent_BindLua, Update),
	lunamethod(Renderable2DComponent_BindLua, Render),
	lunamethod(Renderable2DComponent_BindLua, Compose),
	{ NULL, NULL }
};
Luna<Renderable2DComponent_BindLua>::PropertyType Renderable2DComponent_BindLua::properties[] = {
	{ NULL, NULL }
};

Renderable2DComponent_BindLua::Renderable2DComponent_BindLua(Renderable2DComponent* component)
{
	this->component = component;
}

Renderable2DComponent_BindLua::Renderable2DComponent_BindLua(lua_State *L)
{
	component = new Renderable2DComponent();
}


Renderable2DComponent_BindLua::~Renderable2DComponent_BindLua()
{
}

int Renderable2DComponent_BindLua::AddSprite(lua_State *L)
{
	if (component == nullptr)
	{
		wiLua::SError(L, "AddSprite() component is empty!");
		return 0;
	}
	int argc = wiLua::SGetArgCount(L);
	if (argc > 0)
	{
		wiSprite_BindLua* sprite = Luna<wiSprite_BindLua>::lightcheck(L, 1);
		if (sprite != nullptr)
		{
			Renderable2DComponent* ccomp = dynamic_cast<Renderable2DComponent*>(component);
			if (ccomp != nullptr)
			{
				ccomp->addSprite(sprite->sprite);
			}
			else
			{
				wiLua::SError(L, "AddSprite(Sprite sprite) not a Renderable2DComponent!");
			}
		}
		else
			wiLua::SError(L, "AddSprite(Sprite sprite) argument is not a Sprite!");
	}
	else
	{
		wiLua::SError(L, "AddSprite(Sprite sprite) not enough arguments!");
	}
	return 0;
}
int Renderable2DComponent_BindLua::AddFont(lua_State* L)
{
	if (component == nullptr)
	{
		wiLua::SError(L, "AddFont() component is empty!");
		return 0;
	}
	int argc = wiLua::SGetArgCount(L);
	if (argc > 0)
	{
		wiFont_BindLua* font = Luna<wiFont_BindLua>::lightcheck(L, 1);
		if (font != nullptr)
		{
			Renderable2DComponent* ccomp = dynamic_cast<Renderable2DComponent*>(component);
			if (ccomp != nullptr)
			{
				ccomp->addFont(font->font);
			}
			else
			{
				wiLua::SError(L, "AddFont(Font font) not a Renderable2DComponent!");
			}
		}
		else
			wiLua::SError(L, "AddFont(Font font) argument is not a Font!");
	}
	else
	{
		wiLua::SError(L, "AddFont(Font font) not enough arguments!");
	}
	return 0;
}
int Renderable2DComponent_BindLua::RemoveSprite(lua_State *L)
{
	if (component == nullptr)
	{
		wiLua::SError(L, "RemoveSprite() component is empty!");
		return 0;
	}
	int argc = wiLua::SGetArgCount(L);
	if (argc > 0)
	{
		wiSprite_BindLua* sprite = Luna<wiSprite_BindLua>::lightcheck(L, 1);
		if (sprite != nullptr)
		{
			Renderable2DComponent* ccomp = dynamic_cast<Renderable2DComponent*>(component);
			if (ccomp != nullptr)
			{
				ccomp->removeSprite(sprite->sprite);
			}
			else
			{
				wiLua::SError(L, "RemoveSprite(Sprite sprite) not a Renderable2DComponent!");
			}
		}
		else
			wiLua::SError(L, "RemoveSprite(Sprite sprite) argument is not a Sprite!");
	}
	else
	{
		wiLua::SError(L, "RemoveSprite(Sprite sprite) not enough arguments!");
	}
	return 0;
}
int Renderable2DComponent_BindLua::RemoveFont(lua_State* L)
{
	if (component == nullptr)
	{
		wiLua::SError(L, "RemoveFont() component is empty!");
		return 0;
	}
	int argc = wiLua::SGetArgCount(L);
	if (argc > 0)
	{
		wiFont_BindLua* font = Luna<wiFont_BindLua>::lightcheck(L, 1);
		if (font != nullptr)
		{
			Renderable2DComponent* ccomp = dynamic_cast<Renderable2DComponent*>(component);
			if (ccomp != nullptr)
			{
				ccomp->removeFont(font->font);
			}
			else
			{
				wiLua::SError(L, "RemoveFont(Font font) not a Renderable2DComponent!");
			}
		}
		else
			wiLua::SError(L, "RemoveFont(Font font) argument is not a Font!");
	}
	else
	{
		wiLua::SError(L, "RemoveFont(Font font) not enough arguments!");
	}
	return 0;
}
int Renderable2DComponent_BindLua::ClearSprites(lua_State *L)
{
	if (component == nullptr)
	{
		wiLua::SError(L, "ClearSprites() component is empty!");
		return 0;
	}
	Renderable2DComponent* ccomp = dynamic_cast<Renderable2DComponent*>(component);
	if (ccomp != nullptr)
	{
		ccomp->clearSprites();
	}
	else
	{
		wiLua::SError(L, "ClearSprites() not a Renderable2DComponent!");
	}
	return 0;
}
int Renderable2DComponent_BindLua::ClearFonts(lua_State* L)
{
	if (component == nullptr)
	{
		wiLua::SError(L, "ClearFonts() component is empty!");
		return 0;
	}
	Renderable2DComponent* ccomp = dynamic_cast<Renderable2DComponent*>(component);
	if (ccomp != nullptr)
	{
		ccomp->clearFonts();
	}
	else
	{
		wiLua::SError(L, "ClearFonts() not a Renderable2DComponent!");
	}
	return 0;
}

void Renderable2DComponent_BindLua::Bind()
{
	static bool initialized = false;
	if (!initialized)
	{
		initialized = true;
		Luna<Renderable2DComponent_BindLua>::Register(wiLua::GetGlobal()->GetLuaState());
	}
}