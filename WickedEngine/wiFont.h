#pragma once 
#include "CommonInclude.h"
#include "wiGraphicsDevice.h"
#include "wiColor.h"

struct wiFontParams;

namespace wiFont
{
	void Initialize();
	void CleanUp();

	void LoadShaders();
	const wiGraphics::Texture2D* GetAtlas();

	// Returns the font path that can be modified
	std::string& GetFontPath();

	// Create a font. Returns fontStyleID that is reusable. If font already exists, just return its ID
	int AddFontStyle(const std::string& fontName);

	void Draw(const std::wstring& text, const wiFontParams& params, GRAPHICSTHREAD threadID);

	int ComputeTextWidth(const std::wstring& text, const wiFontParams& params);
	int ComputeTextHeight(const std::wstring& text, const wiFontParams& params);

	void SetText(std::wstring& text_dest, const std::string& text_src);
};

// Do not alter order because it is bound to lua manually
enum wiFontAlign
{
	WIFALIGN_LEFT,
	WIFALIGN_CENTER,
	WIFALIGN_RIGHT,
	WIFALIGN_TOP,
	WIFALIGN_BOTTOM
};

struct wiFontParams
{
	int style = 0;
	int posX = 0;
	int posY = 0;
	int size = 16; // line height in pixels
	float scaling = 1;
	int spacingX = 0;
	int spacingY = 0;
	wiFontAlign h_align = WIFALIGN_LEFT;
	wiFontAlign v_align = WIFALIGN_TOP;
	wiColor color = wiColor(255, 255, 255, 255);
	wiColor shadowColor = wiColor(0, 0, 0, 0);
};
