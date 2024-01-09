#ifndef __VTX_RENDERER_PASSES__
#define __VTX_RENDERER_PASSES__

#include "descriptors.hpp"
#include <util/constants.hpp>
#include <util/math.hpp>

namespace VTX::Renderer
{
	// Attachments.
	static const Attachment imageRGBA32UI { E_FORMAT::RGBA32UI };
	static const Attachment imageRGBA16F { E_FORMAT::RGBA16F };
	static const Attachment imageRG32UI { E_FORMAT::RG32UI };
	static const Attachment imageD32F { E_FORMAT::DEPTH_COMPONENT32F };
	static const Attachment imageR32F { E_FORMAT::R32F };
	static const Attachment imageR16F { E_FORMAT::R16F };
	static const Attachment imageR8 { E_FORMAT::R8 };

	// Data.
	static const Data dataMolecules { { { "Positions", E_TYPE::FLOAT, 3 },
										{ "Colors", E_TYPE::FLOAT, 4 },
										{ "Radii", E_TYPE::FLOAT, 1 },
										{ "Ids", E_TYPE::UINT, 1 },
										{ "Flags", E_TYPE::UINT, 1 } } };

	static const Data dataMeshes { {
		{ "Positions", E_TYPE::FLOAT, 3 },
		{ "Normales", E_TYPE::FLOAT, 3 },
		{ "Colors", E_TYPE::FLOAT, 4 },
		{ "Ids", E_TYPE::UINT, 1 },
		{ "Flags", E_TYPE::UINT, 1 },
	} };

	// Passes.

	// Geometric.
	static const Pass descPassGeometric {
		"Geometric",
		Inputs { { E_CHANNEL_INPUT::_0, { "Molecules", dataMolecules } },
				 { E_CHANNEL_INPUT::_1, { "Meshes", dataMeshes } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "Geometry", imageRGBA32UI } },
				  { E_CHANNEL_OUTPUT::COLOR_1, { "Color", imageRGBA16F } },
				  { E_CHANNEL_OUTPUT::COLOR_2, { "Picking", imageRG32UI } },
				  { E_CHANNEL_OUTPUT::DEPTH, { "Depth", imageD32F } } },
		Programs { { "Sphere", "sphere", Uniforms {}, Draw { "Molecules", E_PRIMITIVE::POINTS, nullptr } },
				   { "Cylinder", "cylinder", Uniforms {}, Draw { "Molecules", E_PRIMITIVE::LINES, nullptr, true } } },
		{ E_SETTING::CLEAR }
	};

	// Linearize depth.
	static const Pass descPassDepth {
		"Linearize depth",
		Inputs { { E_CHANNEL_INPUT::_0, { "Depth", imageD32F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageR32F } } },
		Programs { { "LinearizeDepth", std::vector<FilePath> { "default.vert", "linearize_depth.frag" } } }
	};

	// SSAO.
	static constexpr size_t	  noiseTextureSize = 64;
	static std::vector<Vec3f> createNoiseTexture()
	{
		std::vector<Vec3f> noise( noiseTextureSize * noiseTextureSize );

		std::generate(
			noise.begin(),
			noise.end(),
			[]
			{
				return Util::Math::normalize(
					Vec3f( Util::Math::randomFloat() * 2.f - 1.f, Util::Math::randomFloat() * 2.f - 1.f, 0.f )
				);
			}
		);

		return noise;
	}
	static const std::vector<Vec3f> noiseTexture = createNoiseTexture();

	static const Pass descPassSSAO {
		"SSAO",
		Inputs { { E_CHANNEL_INPUT::_0, { "Geometry", imageRGBA32UI } },
				 { E_CHANNEL_INPUT::_1,
				   { "Noise",
					 Attachment { E_FORMAT::RGB16F,
								  E_WRAPPING::REPEAT,
								  E_WRAPPING::REPEAT,
								  E_FILTERING::NEAREST,
								  E_FILTERING::NEAREST,
								  noiseTextureSize,
								  noiseTextureSize,
								  (void *)( noiseTexture.data() ) } } } // namespace VTX::Renderer
				 ,
				 { E_CHANNEL_INPUT::_2, { "Depth", imageR32F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageR8 } } },
		Programs {
			{ "SSAO",
			  std::vector<FilePath> { "default.vert", "ssao.frag" },
			  Uniforms { { "Intensity",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 1.f, 20.f } } } } } }
	};

	// Blur.
	static const Pass descPassBlur {
		"Blur",
		Inputs { { E_CHANNEL_INPUT::_0, { "Color", imageRGBA16F } }, { E_CHANNEL_INPUT::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageR16F } } },
		Programs {
			{ "Blur",
			  std::vector<FilePath> { "default.vert", "blur.frag" },
			  Uniforms { { "Direction", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i( 1, 0 ) } },
						 { "Size",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 17.f, StructUniformValue<float>::MinMax { 1.f, 99.f } } } } } }
	};

	// Shading.
	static const Pass descPassShading {
		"Shading",
		Inputs { { E_CHANNEL_INPUT::_0, { "Geometry", imageRGBA32UI } },
				 { E_CHANNEL_INPUT::_1, { "Color", imageRGBA16F } },
				 { E_CHANNEL_INPUT::_2, { "Blur", imageR16F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Shading",
			  std::vector<FilePath> { "default.vert", "shading.frag" },
			  Uniforms {
				  { "Background color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_BLACK } },
				  { "Light color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
				  { "Fog color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
				  { "Mode",
					E_TYPE::INT,
					StructUniformValue<int> {
						int( E_SHADING::DIFFUSE ),
						StructUniformValue<int>::MinMax { int( E_SHADING::DIFFUSE ), int( E_SHADING::FLAT_COLOR ) } } },
				  { "Specular factor",
					E_TYPE::FLOAT,
					StructUniformValue<float> { 0.4f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
				  { "Toon steps",
					E_TYPE::UINT,
					StructUniformValue<uint> { 4, StructUniformValue<uint>::MinMax { 1, 15 } } },
				  { "Fog near",
					E_TYPE::FLOAT,
					StructUniformValue<float> { 30.f, StructUniformValue<float>::MinMax { 0.f, 1000.f } } },
				  { "Fog far",
					E_TYPE::FLOAT,
					StructUniformValue<float> { 80.f, StructUniformValue<float>::MinMax { 0.f, 1000.f } } },
				  { "Fog density",
					E_TYPE::FLOAT,
					StructUniformValue<float> { 0.f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
			  } } }
	};

	// Outline.
	static const Pass descPassOutline {
		"Outline",
		Inputs { { E_CHANNEL_INPUT::_0, { "Color", imageRGBA16F } }, { E_CHANNEL_INPUT::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
		Programs { { "Outline",
					 std::vector<FilePath> { "default.vert", "outline.frag" },
					 Uniforms { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
								{ "Sensitivity",
								  E_TYPE::FLOAT,
								  StructUniformValue<float> { 0.f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
								{ "Thickness",
								  E_TYPE::UINT,
								  StructUniformValue<uint> { 1, StructUniformValue<uint>::MinMax { 1, 5 } } } } } }
	};

	// FXAA.
	static const Pass desPassFXAA { "FXAA",
									Inputs { { E_CHANNEL_INPUT::_0, { "Image", imageRGBA16F } } },
									Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
									Programs { { "FXAA", std::vector<FilePath> { "default.vert", "fxaa.frag" } } } };

	// Pixelize.
	static const Pass descPassPixelize {
		"Pixelize",
		Inputs { { E_CHANNEL_INPUT::_0, { "Geometry", imageRGBA32UI } },
				 { E_CHANNEL_INPUT::_1, { "Color", imageRGBA16F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Pixelize",
			  std::vector<FilePath> { "default.vert", "pixelize.frag" },
			  Uniforms {
				  { "Size", E_TYPE::UINT, StructUniformValue<uint> { 5, StructUniformValue<uint>::MinMax { 1, 15 } } },
				  { "Background", E_TYPE::BOOL, StructUniformValue<bool> { true } } } } }
	};

	// CRT.
	static const Pass descPassCRT {
		"CRT",
		Inputs { { E_CHANNEL_INPUT::_0, { "Color", imageRGBA16F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "CRT",
			  std::vector<FilePath> { "default.vert", "crt.frag" },
			  Uniforms { { "Curvature", E_TYPE::VEC2F, StructUniformValue<Vec2f> { Vec2f( 3.f, 3.f ) } },
						 { "Ratio",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 0.25f, StructUniformValue<float>::MinMax { 0.1f, 1.f } } },
						 { "Graniness X",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 0.5f, StructUniformValue<float>::MinMax { 0.f, 5.f } } },
						 { "Graniness Y",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 0.5f, StructUniformValue<float>::MinMax { 0.f, 5.f } } },
						 { "Vignette roundness",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 100.f, StructUniformValue<float>::MinMax { 1.f, 1000.f } } },
						 { "Vignette intensity",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 0.5f, StructUniformValue<float>::MinMax { 0.f, 5.f } } },
						 { "Brightness",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 1.2f, StructUniformValue<float>::MinMax { 1.f, 10.f } } } } } }
	};

	// Chromatic aberration.
	static const Pass descPassChromaticAberration {
		"Chromatic aberration",
		Inputs { { E_CHANNEL_INPUT::_0, { "", imageRGBA16F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Chromatic aberration",
			  std::vector<FilePath> { "default.vert", "chromatic_aberration.frag" },
			  Uniforms {
				  { "Red",
					E_TYPE::FLOAT,
					StructUniformValue<float> { 0.009f, StructUniformValue<float>::MinMax { -0.05f, 0.05f } } },
				  { "Green",
					E_TYPE::FLOAT,
					StructUniformValue<float> { 0.006f, StructUniformValue<float>::MinMax { -0.05f, 0.05f } } },
				  { "Blue",
					E_TYPE::FLOAT,
					StructUniformValue<float> { -0.006f, StructUniformValue<float>::MinMax { -0.05f, 0.05f } } } } } }
	};

	// Colorize.
	static const Pass descPassColorize {
		"Colorize",
		Inputs { { E_CHANNEL_INPUT::_0, { "", imageRGBA16F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Colorize",
			  std::vector<FilePath> { "default.vert", "colorize.frag" },
			  Uniforms { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_YELLOW } } } } }
	};

	// Debug.
	static const Pass descPassDebug {
		"Debug",
		Inputs { { E_CHANNEL_INPUT::_0, { "", imageRGBA16F } } },
		Outputs { { E_CHANNEL_OUTPUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Debug",
			  std::vector<FilePath> { "default.vert", "debug.frag" },
			  Uniforms { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_YELLOW } },
						 { "Color2", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_BLUE } },
						 { "Test", E_TYPE::FLOAT, StructUniformValue<float> { 5646.f } },
						 { "Factor",
						   E_TYPE::FLOAT,
						   StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 0.f, 10.f } } } } } }
	};

	static const std::vector<Pass> availablePasses {
		descPassGeometric, descPassDepth, descPassSSAO,		descPassBlur, descPassShading,
		descPassOutline,   desPassFXAA,	  descPassPixelize, descPassCRT,  descPassChromaticAberration,
		descPassColorize,  descPassDebug
	};
} // namespace VTX::Renderer

#endif
