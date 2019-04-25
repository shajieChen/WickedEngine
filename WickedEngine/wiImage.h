#pragma once
#include "CommonInclude.h"
#include "wiGraphicsDevice.h"
#include "wiEnums.h"

struct wiImageParams;

namespace wiImage
{
	void Draw(const wiGraphics::Texture2D* texture, const wiImageParams& params, GRAPHICSTHREAD threadID);

	void DrawDeferred(
		const wiGraphics::Texture2D* lightmap_diffuse, 
		const wiGraphics::Texture2D* lightmap_specular,
		const wiGraphics::Texture2D* ao, 
		GRAPHICSTHREAD threadID, 
		int stencilref = 0);

	void LoadShaders();
	void Initialize();
};

enum STENCILMODE
{
	STENCILMODE_DISABLED,
	STENCILMODE_GREATER,
	STENCILMODE_LESS,
	STENCILMODE_EQUAL,
	STENCILMODE_COUNT
};
enum SAMPLEMODE
{
	SAMPLEMODE_CLAMP,
	SAMPLEMODE_WRAP,
	SAMPLEMODE_MIRROR,
	SAMPLEMODE_COUNT
};
enum QUALITY
{
	QUALITY_NEAREST,
	QUALITY_LINEAR,
	QUALITY_ANISOTROPIC,
	QUALITY_BICUBIC,
	QUALITY_COUNT
};
enum ImageType
{
	SCREEN,
	WORLD,
};

struct wiImageParams
{
	enum FLAGS
	{
		EMPTY = 0,
		DRAWRECT = 1 << 0,
		MIRROR = 1 << 1,
		EXTRACT_NORMALMAP = 1 << 2,
		HDR = 1 << 3,
		FULLSCREEN = 1 << 4,
	};
	uint32_t _flags = EMPTY;

	XMFLOAT3 pos = XMFLOAT3(0, 0, 0);
	XMFLOAT2 siz = XMFLOAT2(1, 1);
	XMFLOAT2 scale = XMFLOAT2(1, 1);
	XMFLOAT4 col = XMFLOAT4(1, 1, 1, 1);
	XMFLOAT4 drawRect = XMFLOAT4(0, 0, 0, 0);
	XMFLOAT4 lookAt = XMFLOAT4(0, 0, 0, 0);			//(x,y,z) : direction, (w) :isenabled?
	XMFLOAT2 texOffset = XMFLOAT2(0, 0);
	XMFLOAT2 pivot = XMFLOAT2(0, 0);				// (0,0) : upperleft, (0.5,0.5) : center, (1,1) : bottomright
	float rotation = 0.0f;
	float mipLevel = 0.0f;
	float fade = 0.0f;
	float opacity = 1.0f;
	XMFLOAT2 corners[4] = {
		XMFLOAT2(0, 0),
		XMFLOAT2(1, 0),
		XMFLOAT2(0, 1),
		XMFLOAT2(1, 1),
	};		// you can deform the image by its corners (0: top left, 1: top right, 2: bottom left, 3: bottom right)

	UINT stencilRef = 0;
	STENCILMODE stencilComp = STENCILMODE_DISABLED;
	BLENDMODE blendFlag = BLENDMODE_ALPHA;
	ImageType typeFlag = SCREEN;
	SAMPLEMODE sampleFlag = SAMPLEMODE_MIRROR;
	QUALITY quality = QUALITY_LINEAR;

	const wiGraphics::Texture2D* maskMap = nullptr;
	const wiGraphics::Texture2D* distortionMap = nullptr;

	// The texture will be multiplied by this texture
	void setMaskMap(const wiGraphics::Texture2D* value) { maskMap = value; }
	// The normalmap texture which should distort the texture
	void setDistortionMap(const wiGraphics::Texture2D* value) { distortionMap = value; }

	constexpr bool isDrawRectEnabled() const { return _flags & DRAWRECT; }
	constexpr bool isMirrorEnabled() const { return _flags & MIRROR; }
	constexpr bool isExtractNormalMapEnabled() const { return _flags & EXTRACT_NORMALMAP; }
	constexpr bool isHDREnabled() const { return _flags & HDR; }
	constexpr bool isFullScreenEnabled() const { return _flags & FULLSCREEN; }

	// enables draw rectangle (cutout texture outside draw rectangle)
	void enableDrawRect(const XMFLOAT4& rect) { _flags |= DRAWRECT; drawRect = rect; }
	// mirror the image horizontally
	void enableMirror() { _flags |= MIRROR; }
	// enable normal map extraction shader that will perform texcolor * 2 - 1 (preferably onto a signed render target)
	void enableExtractNormalMap() { _flags |= EXTRACT_NORMALMAP; }
	// enable HDR blending to float render target
	void enableHDR() { _flags |= HDR; }
	// enable full screen override. It just draws texture onto the full screen, disabling any other setup except sampler and stencil)
	void enableFullScreen() { _flags |= FULLSCREEN; }

	// disable draw rectangle (whole texture will be drawn, no cutout)
	void disableDrawRect() { _flags &= ~DRAWRECT; }
	// disable mirroring
	void disableMirror() { _flags &= ~MIRROR; }
	// disable normal map extraction shader
	void disableExtractNormalMap() { _flags &= ~EXTRACT_NORMALMAP; }
	// if you want to render onto normalized render target
	void disableHDR() { _flags &= ~HDR; }
	// disable full screen override
	void disableFullScreen() { _flags &= ~FULLSCREEN; }

	struct PostProcess
	{
		enum POSTPROCESS
		{
			DISABLED,
			BLUR_LDR,
			BLUR_HDR,
			LIGHTSHAFT,
			OUTLINE,
			DEPTHOFFIELD,
			MOTIONBLUR,
			BLOOMSEPARATE,
			FXAA,
			SSAO,
			SSSS,
			SSR,
			COLORGRADE,
			TONEMAP,
			REPROJECTDEPTHBUFFER,
			DOWNSAMPLEDEPTHBUFFER,
			TEMPORALAA,
			SHARPEN,
			LINEARDEPTH,
			POSTPROCESS_COUNT
		} type = DISABLED;

		union PostProcessParams
		{
			struct Outline
			{
				float colorR;
				float colorG;
				float colorB;
				float threshold;
				float thickness;
			} outline;
			struct Blur
			{
				float x;
				float y;
			} blur;
			Blur ssss;
			Blur sun;
			struct SSAO
			{
				float range;
				UINT sampleCount;
			} ssao;
			float dofFocus;
			float sharpen;
			float exposure;
			float bloomThreshold;
		} params;

		bool isActive() const { return type != DISABLED; }
		void clear() { type = DISABLED; }
		void setBlur(const XMFLOAT2& direction, bool hdr = false) { type = (hdr ? BLUR_HDR : BLUR_LDR); params.blur.x = direction.x; params.blur.y = direction.y; }
		void setBloom(float threshold) { type = BLOOMSEPARATE; params.bloomThreshold = threshold; }
		void setDOF(float focus) { if (focus > 0) { type = DEPTHOFFIELD; params.dofFocus = focus; } }
		void setMotionBlur() { type = MOTIONBLUR; }
		void setOutline(float threshold = 0.1f, float thickness = 1.0f, const XMFLOAT3& color = XMFLOAT3(0, 0, 0))
		{
			type = OUTLINE;
			params.outline.threshold = threshold;
			params.outline.thickness = thickness;
			params.outline.colorR = color.x;
			params.outline.colorG = color.y;
			params.outline.colorB = color.z;
		}
		void setFXAA() { type = FXAA; }
		void setSSAO(float range = 1.0f, UINT sampleCount = 16) { type = SSAO; params.ssao.range = range; params.ssao.sampleCount = sampleCount; }
		void setLinDepth() { type = LINEARDEPTH; }
		void setColorGrade() { type = COLORGRADE; }
		void setSSSS(const XMFLOAT2& value) { type = SSSS; params.ssss.x = value.x; params.ssss.y = value.y; }
		void setSSR() { type = SSR; }
		void setToneMap(float exposure) { type = TONEMAP; params.exposure = exposure; }
		void setDepthBufferReprojection() { type = REPROJECTDEPTHBUFFER; }
		void setDepthBufferDownsampling() { type = DOWNSAMPLEDEPTHBUFFER; }
		void setTemporalAAResolve() { type = TEMPORALAA; }
		void setSharpen(float value) { if (value > 0) { type = SHARPEN; params.sharpen = value; } }
		void setLightShaftCenter(const XMFLOAT2& pos) { type = LIGHTSHAFT; params.sun.x = pos.x; params.sun.y = pos.y; }
	};
	PostProcess process;

	wiImageParams() {}
	wiImageParams(float width, float height) :siz(XMFLOAT2(width, height)) {}
	wiImageParams(float x, float y, float width, float height) :pos(XMFLOAT3(x, y, 0)), siz(XMFLOAT2(width, height)) {}
};
