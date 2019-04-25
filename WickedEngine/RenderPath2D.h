#pragma once
#include "RenderPath.h"
#include "wiGUI.h"
#include "wiSceneSystem_Decl.h"

#include <string>

static const std::string DEFAULT_RENDERLAYER = "default";

struct RenderItem2D
{
	enum TYPE
	{
		SPRITE,
		FONT,
	} type;
	wiSceneSystem::SpriteComponent* sprite = nullptr;
	wiSceneSystem::TextComponent* font = nullptr;
	int order = 0;
};
struct RenderLayer2D
{
	std::vector<RenderItem2D> items;
	std::string name;
	int order = 0;

	RenderLayer2D(const std::string& name) :name(name) {}
};

class RenderPath2D :
	public RenderPath
{
private:
	wiGraphics::Texture2D rtFinal;
	wiGUI GUI;
	float spriteSpeed = 1.0f;

protected:
	void ResizeBuffers() override;
public:
	RenderPath2D();

	void Initialize() override;
	void Load() override;
	void Unload() override;
	void Start() override;
	void Update(float dt) override;
	void FixedUpdate() override;
	void Render() const override;
	void Compose() const override;

	const wiGraphics::Texture2D& GetRenderResult() { return rtFinal; }

	void addSprite(wiSceneSystem::SpriteComponent* sprite, const std::string& layer = DEFAULT_RENDERLAYER);
	void removeSprite(wiSceneSystem::SpriteComponent* sprite);
	void clearSprites();
	void setSpriteSpeed(float value) { spriteSpeed = value; }
	float getSpriteSpeed() { return spriteSpeed; }
	int getSpriteOrder(wiSceneSystem::SpriteComponent* sprite);

	void addFont(wiSceneSystem::TextComponent* font, const std::string& layer = DEFAULT_RENDERLAYER);
	void removeFont(wiSceneSystem::TextComponent* font);
	void clearFonts();
	int getFontOrder(wiSceneSystem::TextComponent* font);

	std::vector<RenderLayer2D> layers;
	void addLayer(const std::string& name);
	void setLayerOrder(const std::string& name, int order);
	void SetSpriteOrder(wiSceneSystem::SpriteComponent* sprite, int order);
	void SetFontOrder(wiSceneSystem::TextComponent* font, int order);
	void SortLayers();
	void CleanLayers();

	const wiGUI& GetGUI() const { return GUI; }
	wiGUI& GetGUI() { return GUI; }
};

