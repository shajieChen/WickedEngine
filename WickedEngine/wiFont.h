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

	void Draw(const std::string& text, const wiFontParams& params, GRAPHICSTHREAD threadID);
	void Draw(const std::wstring& text, const wiFontParams& params, GRAPHICSTHREAD threadID);

	int ComputeTextWidth(const std::string& text, const wiFontParams& params);
	int ComputeTextWidth(const std::wstring& text, const wiFontParams& params);
	int ComputeTextHeight(const std::string& text, const wiFontParams& params);
	int ComputeTextHeight(const std::wstring& text, const wiFontParams& params);

	// Converts UTF-8 encoded string to native wide string that can be rendered
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

static const int WIFONTSIZE_DEFAULT = 16;
struct wiFontParams
{
	int style = 0;
	int posX = 0;
	int posY = 0;
	int size = WIFONTSIZE_DEFAULT; // line height in pixels
	float scaling = 1;
	int spacingX = 0;
	int spacingY = 0;
	wiFontAlign h_align = WIFALIGN_LEFT;
	wiFontAlign v_align = WIFALIGN_TOP;
	wiColor color = wiColor(255, 255, 255, 255);
	wiColor shadowColor = wiColor(0, 0, 0, 0);

	wiFontParams() {}
	wiFontParams(int posX, int posY, int size = WIFONTSIZE_DEFAULT, wiFontAlign h_align = WIFALIGN_LEFT, wiFontAlign v_align = WIFALIGN_TOP
		, int spacingX = 0, int spacingY = 0, const wiColor& color = wiColor(255, 255, 255, 255), const wiColor& shadowColor = wiColor(0, 0, 0, 0))
		:posX(posX), posY(posY), size(size), h_align(h_align), v_align(v_align), spacingX(spacingX), spacingY(spacingY), color(color), shadowColor(shadowColor)
	{}
};
