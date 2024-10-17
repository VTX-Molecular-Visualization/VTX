#ifndef __VTX_RENDERER_PASSES__
#define __VTX_RENDERER_PASSES__

#include "descriptors.hpp"
#include "settings.hpp"
#include <util/constants.hpp>
#include <util/math.hpp>

/**
 * @brief All the currently availables metadata.
 */
namespace VTX::Renderer
{
	// Attachments.
	inline const Attachment imageRGBA32UI { E_FORMAT::RGBA32UI };
	inline const Attachment imageRGBA16F { E_FORMAT::RGBA16F };
	inline const Attachment imageRG32UI { E_FORMAT::RG32UI };
	inline const Attachment imageD32F { E_FORMAT::DEPTH_COMPONENT32F };
	inline const Attachment imageR32F { E_FORMAT::R32F };
	inline const Attachment imageR16F { E_FORMAT::R16F };
	inline const Attachment imageR8 { E_FORMAT::R8 };

	// Data.
	// TODO: compress all.
	inline const Data dataSpheresCylinders { {
		{ "Positions", E_TYPE::FLOAT, 3 },
		{ "Colors", E_TYPE::UBYTE, 1 },
		{ "Radii", E_TYPE::FLOAT, 1 }, // TODO: move to ubo or hardcode?
		{ "Ids", E_TYPE::UINT, 1 },
		{ "Flags", E_TYPE::UBYTE, 1 },
		{ "Models", E_TYPE::USHORT, 1 },
		{ "Representations", E_TYPE::UBYTE, 1 },
	} };

	inline const Data dataRibbons { {
		{ "Positions", E_TYPE::FLOAT, 4 },
		{ "Directions", E_TYPE::FLOAT, 3 },
		{ "Types", E_TYPE::UBYTE, 1 },
		{ "Colors", E_TYPE::UBYTE, 1 },
		{ "Ids", E_TYPE::UINT, 1 },
		{ "Flags", E_TYPE::UBYTE, 1 },
		{ "Models", E_TYPE::USHORT, 1 },
		{ "Representations", E_TYPE::UBYTE, 1 },
	} };

	inline const Data dataTriangles { {
		{ "Positions", E_TYPE::FLOAT, 3 },
		{ "Normales", E_TYPE::FLOAT, 3 },
		{ "Colors", E_TYPE::UBYTE, 1 },
		{ "Ids", E_TYPE::UINT, 1 },
		{ "Flags", E_TYPE::UBYTE, 1 },
		{ "Models", E_TYPE::USHORT, 1 },
		{ "Representations", E_TYPE::UBYTE, 1 },
	} };

	inline const Data dataVoxels { { { "Mins", E_TYPE::FLOAT, 3 }, { "Maxs", E_TYPE::FLOAT, 3 } } };

	// Passes.

	// Geometric.
	inline Pass descPassGeometric {
		"Geometric",
		Inputs { { E_CHAN_IN::_0, { "SpheresCylinders", dataSpheresCylinders } },
				 { E_CHAN_IN::_1, { "Ribbons", dataRibbons } },
				 { E_CHAN_IN::_2, { "Triangles", dataTriangles } },
				 { E_CHAN_IN::_3, { "Voxels", dataVoxels } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "Geometry", imageRGBA32UI } },
				  { E_CHAN_OUT::COLOR_1, { "Color", imageRGBA16F } },
				  { E_CHAN_OUT::COLOR_2, { "Picking", imageRG32UI } },
				  { E_CHAN_OUT::DEPTH, { "Depth", imageD32F } } },
		Programs { { "Sphere", "sphere", Uniforms {}, Draw { "SpheresCylinders", E_PRIMITIVE::POINTS } },
				   { "Cylinder", "cylinder", Uniforms {}, Draw { "SpheresCylinders", E_PRIMITIVE::LINES, true } },
				   { "Ribbon", "ribbon", Uniforms {}, Draw { "Ribbons", E_PRIMITIVE::PATCHES, true } },
				   { "Voxel", "voxel", Uniforms {}, Draw { "Voxels", E_PRIMITIVE::POINTS } } },
		{ E_SETTING::CLEAR }
	};

	// Linearize depth.
	inline Pass descPassDepth { "Linearize depth",
								Inputs { { E_CHAN_IN::_0, { "Depth", imageD32F } } },
								Outputs { { E_CHAN_OUT::COLOR_0, { "", imageR32F } } },
								Programs { { "LinearizeDepth",
											 std::vector<FilePath> { "default.vert", "linearize_depth.frag" } } } };

	// SSAO.
	inline constexpr size_t	  noiseTextureSize = 64;
	inline std::vector<Vec3f> createNoiseTexture()
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
	inline const std::vector<Vec3f> noiseTexture = createNoiseTexture();

	inline Pass descPassSSAO {
		"SSAO",
		Inputs { { E_CHAN_IN::_0, { "Geometry", imageRGBA32UI } },
				 { E_CHAN_IN::_1,
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
				 { E_CHAN_IN::_2, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageR8 } } },
		Programs {
			{ "SSAO",
			  std::vector<FilePath> { "default.vert", "ssao.frag" },
			  Uniforms {

				  { { "Intensity",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> {
						  SSAO_INTENSITY_DEFAULT,
						  StructUniformValue<float>::MinMax { SSAO_INTENSITY_MIN, SSAO_INTENSITY_MAX } } } } } } }
	};

	// Blur.
	inline Pass descPassBlur {
		"Blur",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F } }, { E_CHAN_IN::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageR16F } } },
		Programs { { "Blur",
					 std::vector<FilePath> { "default.vert", "blur.frag" },
					 Uniforms {

						 { { "Direction", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i( 1, 0 ) } },
						   { "Size",
							 E_TYPE::FLOAT,
							 StructUniformValue<float> {
								 BLUR_SIZE_DEFAULT,
								 StructUniformValue<float>::MinMax { BLUR_SIZE_MIN, BLUR_SIZE_MAX } } } } } } }
	};

	// Shading.
	inline const std::vector<float> blurDefaultTexture( 1, 1.f );

	inline Pass descPassShading {
		"Shading",
		Inputs { { E_CHAN_IN::_0, { "Geometry", imageRGBA32UI } },
				 { E_CHAN_IN::_1, { "Color", imageRGBA16F } },
				 { E_CHAN_IN::_2,
				   { "Blur",
					 Attachment { E_FORMAT::R16F,
								  E_WRAPPING::REPEAT,
								  E_WRAPPING::REPEAT,
								  E_FILTERING::NEAREST,
								  E_FILTERING::NEAREST,
								  size_t( 1 ),
								  size_t( 1 ),
								  (void *)( blurDefaultTexture.data() ) } } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Shading",
			  std::vector<FilePath> { "default.vert", "shading.frag" },
			  Uniforms {

				  {
					  { "Background color",
						E_TYPE::COLOR4,
						StructUniformValue<Util::Color::Rgba> { COLOR_BACKGROUND_DEFAULT } },
					  { "Light color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_LIGHT_DEFAULT } },
					  { "Fog color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_FOG_DEFAULT } },
					  { "Mode",
						E_TYPE::UINT,
						StructUniformValue<uint> {
							uint( SHADING_MODE_DEFAULT ),
							StructUniformValue<uint>::MinMax { uint( E_SHADING::DIFFUSE ),
															   uint( E_SHADING::COUNT ) - 1 } } },
					  { "Specular factor",
						E_TYPE::FLOAT,
						StructUniformValue<float> {
							SPECULAR_FACTOR_DEFAULT,
							StructUniformValue<float>::MinMax { SPECULAR_FACTOR_MIN, SPECULAR_FACTOR_MAX } } },
					  { "Shininess",
						E_TYPE::FLOAT,
						StructUniformValue<float> {
							SHININESS_DEFAULT,
							StructUniformValue<float>::MinMax { SHININESS_MIN, SHININESS_MAX } } },
					  { "Toon steps",
						E_TYPE::UINT,
						StructUniformValue<uint> {
							TOON_STEPS_DEFAULT,
							StructUniformValue<uint>::MinMax { TOON_STEPS_MIN, TOON_STEPS_MAX } } },
					  { "Fog near",
						E_TYPE::FLOAT,
						StructUniformValue<float> {
							FOG_NEAR_DEFAULT,
							StructUniformValue<float>::MinMax { FOG_NEAR_MIN, FOG_NEAR_MAX } } },
					  { "Fog far",
						E_TYPE::FLOAT,
						StructUniformValue<float> { FOG_FAR_DEFAULT,
													StructUniformValue<float>::MinMax { FOG_FAR_MIN, FOG_FAR_MAX } } },
					  { "Fog density",
						E_TYPE::FLOAT,
						StructUniformValue<float> {
							FOG_DENSITY_DEFAULT,
							StructUniformValue<float>::MinMax { FOG_DENSITY_MIN, FOG_DENSITY_MAX } } },
				  } } } }
	};

	// Outline.
	inline Pass descPassOutline {
		"Outline",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F } }, { E_CHAN_IN::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Outline",
			  std::vector<FilePath> { "default.vert", "outline.frag" },
			  Uniforms {
				  { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
					{ "Sensitivity",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> {
						  OUTLINE_SENSITIVITY_DEFAULT,
						  StructUniformValue<float>::MinMax { OUTLINE_SENSITIVITY_MIN, OUTLINE_SENSITIVITY_MAX } } },
					{ "Thickness",
					  E_TYPE::UINT,
					  StructUniformValue<uint> {
						  OUTLINE_THICKNESS_DEFAULT,
						  StructUniformValue<uint>::MinMax { OUTLINE_THICKNESS_MIN, OUTLINE_THICKNESS_MAX } } } } } } }
	};

	// Glow.
	inline Pass descPassGlow {
		"Glow",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F } }, { E_CHAN_IN::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs { { "Gow",
					 std::vector<FilePath> { "default.vert", "glow.frag" },
					 Uniforms { { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
								  { "Sensitivity",
									E_TYPE::FLOAT,
									StructUniformValue<float> { 0.f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
								  { "Size",
									E_TYPE::UINT,
									StructUniformValue<uint> { 1, StructUniformValue<uint>::MinMax { 1, 5 } } } } } } }
	};

	// Selection.
	inline Pass descPassSelection {
		"Selection",
		Inputs { { E_CHAN_IN::_0, { "Geometry", imageRGBA32UI } },
				 { E_CHAN_IN::_1, { "Color", imageRGBA16F } },
				 { E_CHAN_IN::_2, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs { { "Selection",
					 std::vector<FilePath> { "default.vert", "selection.frag" },
					 Uniforms { { { "Color",
									E_TYPE::COLOR4,
									StructUniformValue<Util::Color::Rgba> { COLOR_SELECTION_DEFAULT } } } } } }
	};

	// FXAA.
	inline Pass desPassFXAA { "FXAA",
							  Inputs { { E_CHAN_IN::_0, { "Image", imageRGBA16F } } },
							  Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
							  Programs { { "FXAA", std::vector<FilePath> { "default.vert", "fxaa.frag" } } } };

	// Pixelize.
	inline Pass descPassPixelize {
		"Pixelize",
		Inputs { { E_CHAN_IN::_0, { "Geometry", imageRGBA32UI } }, { E_CHAN_IN::_1, { "Color", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs { { "Pixelize",
					 std::vector<FilePath> { "default.vert", "pixelize.frag" },
					 Uniforms { { { "Size",
									E_TYPE::UINT,
									StructUniformValue<uint> { 5, StructUniformValue<uint>::MinMax { 1, 15 } } },
								  { "Background", E_TYPE::BOOL, StructUniformValue<bool> { true } } } } } }
	};

	// CRT.
	inline Pass descPassCRT {
		"CRT",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "CRT",
			  std::vector<FilePath> { "default.vert", "crt.frag" },
			  Uniforms {

				  { { "Curvature", E_TYPE::VEC2F, StructUniformValue<Vec2f> { Vec2f( 3.f, 3.f ) } },
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
					  StructUniformValue<float> { 1.2f, StructUniformValue<float>::MinMax { 1.f, 10.f } } } } } } }
	};

	// Chromatic aberration.
	inline Pass descPassChromaticAberration {
		"Chromatic aberration",
		Inputs { { E_CHAN_IN::_0, { "", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Chromatic aberration",
			  std::vector<FilePath> { "default.vert", "chromatic_aberration.frag" },
			  Uniforms {

				  { { "Red",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 0.009f, StructUniformValue<float>::MinMax { -0.05f, 0.05f } } },
					{ "Green",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 0.006f, StructUniformValue<float>::MinMax { -0.05f, 0.05f } } },
					{ "Blue",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { -0.006f,
												  StructUniformValue<float>::MinMax { -0.05f, 0.05f } } } } } } }
	};

	// Colorize.
	inline Pass descPassColorize {
		"Colorize",
		Inputs { { E_CHAN_IN::_0, { "", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Colorize",
			  std::vector<FilePath> { "default.vert", "colorize.frag" },
			  Uniforms { { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_YELLOW } } } } } }
	};

	// Debug.
	inline Pass descPassDebug {
		"Debug",
		Inputs { { E_CHAN_IN::_0, { "", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs {
			{ "Debug",
			  std::vector<FilePath> { "default.vert", "debug.frag" },
			  Uniforms {

				  { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_YELLOW } },
					{ "Color2", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_BLUE } },
					{ "Test", E_TYPE::FLOAT, StructUniformValue<float> { 5646.f } },
					{ "Factor",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 0.f, 10.f } } } } } } }
	};

	enum class E_PASS : size_t
	{
		GEOMETRIC,
		DEPTH,
		SSAO,
		BLUR,
		SHADING,
		OUTLINE,
		SELECTION,
		FXAA,
		PIXELIZE,
		CRT,
		CHROMATIC_ABERRATION,
		COLORIZE,
		DEBUG,
		COUNT
	};

	inline std::vector<Pass *> availablePasses = { &descPassGeometric,
												   &descPassDepth,
												   &descPassSSAO,
												   &descPassBlur,
												   &descPassShading,
												   &descPassOutline,
												   &descPassSelection,
												   &desPassFXAA,
												   &descPassPixelize,
												   &descPassCRT,
												   &descPassChromaticAberration,
												   &descPassColorize,
												   &descPassDebug };
} // namespace VTX::Renderer

#endif
