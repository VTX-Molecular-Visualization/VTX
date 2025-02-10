#ifndef __VTX_RENDERER_PASSES__
#define __VTX_RENDERER_PASSES__

#include "descriptors.hpp"
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
	inline const Attachment imageSmall { E_FORMAT::R8,
										E_WRAPPING::CLAMP_TO_EDGE,
										E_WRAPPING::CLAMP_TO_EDGE,
										E_FILTERING::NEAREST,
										E_FILTERING::NEAREST,
										0.5f,
										0.5f,
										nullptr };
	inline const Attachment imageRGBA16F_small { E_FORMAT::RGBA16F,
												  E_WRAPPING::CLAMP_TO_EDGE,
												  E_WRAPPING::CLAMP_TO_EDGE,
												  E_FILTERING::NEAREST,
												  E_FILTERING::NEAREST,
												  0.5f,
												  0.5f,
												  nullptr };

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
	
	/* //inline Pass descPassLogDepth{ "Log scale depth",
								Inputs { { E_CHAN_IN::_0, { "Depth", imageD32F } } },
								Outputs { { E_CHAN_OUT::COLOR_0, { "", imageR32F } } },
								Programs { { "LogDepth",
											 std::vector<FilePath> { "default.vert", "log_depth.frag" } } } };
*/
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
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageSmall } }
					},
		Programs {
			{ "SSAO",
			  std::vector<FilePath> { "default.vert", "ssao.frag" },
			  Uniforms {

				  { { "Intensity",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 1.f, 20.f } } },
					  { "Bias",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 0.0f, 1.f } } }
					   } } } }
	};

		// Scale
	inline Pass descPassScale {
		"Scale",
		Inputs { { E_CHAN_IN::_0, { "Color", imageSmall } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageR8 } } },
		//change it back to 16f
		Programs {
			{ "Scale",
			  std::vector<FilePath> { "default.vert", "scale.frag" },
			   } }
	};

			// Scale
	inline Pass descPassScaleRGBA {
		"Scale RGBA",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F_small } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		//change it back to 16f
		Programs {
			{ "Scale",
			  std::vector<FilePath> { "default.vert", "scale_rgba.frag" },
			   } }
	};

	// Blur.
	inline Pass descPassBlur {
		"Blur",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F } }, { E_CHAN_IN::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageR16F } } },
		Programs {
			{ "Blur",
			  std::vector<FilePath> { "default.vert", "blur.frag" },
			  Uniforms {

				  { { "Direction", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i( 1, 0 ) } },
					{ "Size",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 17.f, StructUniformValue<float>::MinMax { 1.f, 99.f } } } } } } }
	};

		// Blur.
	inline Pass descPassBlurSmall {
		"Blur",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F_small } }, { E_CHAN_IN::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F_small } } },
		Programs {
			{ "Blur",
			  std::vector<FilePath> { "default.vert", "blur.frag" },
			  Uniforms {

				  { { "Direction", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i( 1, 0 ) } },
					{ "Size",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 17.f, StructUniformValue<float>::MinMax { 1.f, 99.f } } } } } } }
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
					  { "Background color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_BLACK } },
					  { "Light color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
					  { "Fog color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
					  { "Mode",
						E_TYPE::UINT,
						StructUniformValue<uint> {
							uint( E_SHADING::DIFFUSE ),
							StructUniformValue<uint>::MinMax { uint( E_SHADING::DIFFUSE ),
															   uint( E_SHADING::COUNT ) - 1 } } },
					  { "Specular factor",
						E_TYPE::FLOAT,
						StructUniformValue<float> { 0.4f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
					  { "Shininess",
						E_TYPE::FLOAT,
						StructUniformValue<float> { 32.f, StructUniformValue<float>::MinMax { 0.f, 128.f } } },
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
				  } } } }
	};

	// Outline.
	inline Pass descPassOutline {
		"Outline",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F } }, { E_CHAN_IN::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageRGBA16F } } },
		Programs { { "Outline",
					 std::vector<FilePath> { "default.vert", "outline.frag" },
					 Uniforms { { { "Color", E_TYPE::COLOR4, StructUniformValue<Util::Color::Rgba> { COLOR_WHITE } },
								  { "Sensitivity",
									E_TYPE::FLOAT,
									StructUniformValue<float> { 0.f, StructUniformValue<float>::MinMax { 0.f, 1.f } } },
								  { "Thickness",
									E_TYPE::UINT,
									StructUniformValue<uint> { 1, StructUniformValue<uint>::MinMax { 1, 5 } } } } } } }
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
									StructUniformValue<Util::Color::Rgba> { Util::Color::Rgba( 45, 243, 26 ) } } } } } }
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

	//SSAO LINE
		inline Pass descPassSSAO_LINE {
		"SSAO_LINE",
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
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageSmall } } 	
					},
		Programs {
			{ "SSAO_LINE",
			  std::vector<FilePath> { "default.vert", "ssao_line.frag" },
			  Uniforms {

				  { { "Intensity",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 1.f, 20.f } } },
					  { "Radius",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 0.5f, 20.f } } }

					   } } } }
	};

	//HBAO
		inline Pass descPassHBAO {
		"HBAO",
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
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageSmall } } 
					}, 
		Programs {
			{ "HBAO",
			  std::vector<FilePath> { "default.vert", "hbao.frag" },
			  Uniforms {

				  { { "Intensity",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 1.f, 20.f } } },
					
					  { "Bias",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 0.f, StructUniformValue<float>::MinMax { 0.f, 0.2f } } }					  
					   } } } }
	};

	//SAO
		inline Pass descPassSAO {
		"SAO",
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
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageSmall } }
					},
		Programs {
			{ "SAO",
			  std::vector<FilePath> { "default.vert", "sao.frag" },
			  Uniforms {

				  { { "Intensity",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 1.f, 20.f } } },
					  { "Radius",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 0.5f, 20.f } } }
					   } } } }
	};

inline Pass descPassBMGTAO {
		"BitMask AO",
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
				 { E_CHAN_IN::_2, { "Depth", imageR32F } },
				 { E_CHAN_IN::_3, { "Color", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "AO", imageSmall } },
					{ E_CHAN_OUT::COLOR_1, { "", imageR32F } },
					{ E_CHAN_OUT::COLOR_2, { "ColorBleed", imageRGBA16F_small } }
					},
		Programs {
			{ "BitMask AO",
			  std::vector<FilePath> { "default.vert", "bmgtao.frag" },
			  Uniforms {

				  { { "Intensity",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 5.f, StructUniformValue<float>::MinMax { 1.f, 10.f } } },
					  { "Radius",
					  E_TYPE::FLOAT,
					  StructUniformValue<float> { 300.f, StructUniformValue<float>::MinMax { 0.0f, 1000.f } } }
					   } } } }
	};

	inline Pass descPassSumImages {
		"Sum images",
		Inputs { { E_CHAN_IN::_0, { "Image 1", imageRGBA16F } }, { E_CHAN_IN::_1, { "Image 2", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "Sum", imageRGBA16F } } },
		Programs {
			{ "Sum images",
			  std::vector<FilePath> { "default.vert", "sum_images.frag" },
			  Uniforms{
				{
					{ "Image1 intensity",
					 E_TYPE::FLOAT, 
					 StructUniformValue<float> { 0.5f, StructUniformValue<float>::MinMax { 0.0f, 1.f }  }}
			
				}
			  } } }
	};

	// Blur.
/*	inline Pass descPassIntegralImage {// -> has to be done on cpu figure it out somehow
		"Summed tables",
		Inputs { { E_CHAN_IN::_0, { "Color", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "summed", imageRGBA16F } } },
		Programs {
			{ "Summed tables",
			  std::vector<FilePath> { "default.vert", "integral_image.frag" } } }
	}; */

	/* // Blur.
	inline Pass descPassPrecomputeTables {
		"Precompute",
		Inputs {{E_CHAN_IN::_0, { "Geometry", imageRGBA32UI } },
				{ E_CHAN_IN::_1, { "Color", imageR8 } }, 
				{ E_CHAN_IN::_2, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "j_k", imageRGBA16F } },
				  { E_CHAN_OUT::COLOR_1, { "w_k", imageRGBA16F } } },
		Programs {
			{ "Precompute values",
			  std::vector<FilePath> { "default.vert", "j_k.frag" },
			  Uniforms{
				{{"Minimum depth", E_TYPE::FLOAT, StructUniformValue<float> { 20.f }},
				{"Variance", 
				E_TYPE::FLOAT, 
				StructUniformValue<float> {0.01f,StructUniformValue<float>::MinMax {0.005f, 0.1f}}}
				}}
			   } }
	};

	// Blur.
	inline Pass descPassInterpolateBlur { 
		"Interpolate",
		Inputs { { E_CHAN_IN::_0, { "jbk", imageRGBA16F } }, { E_CHAN_IN::_1, { "Depth", imageR32F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "", imageR16F } } },
		Programs {
			{ "Interpolate",
			  std::vector<FilePath> { "default.vert", "interpolate.frag" } } }
	};

	// Blur.
	inline Pass descPassBoxFilter {
		"range filter",
		Inputs { { E_CHAN_IN::_0, { "jk_summed", imageRGBA16F } }, { E_CHAN_IN::_1, { "wk_summed", imageRGBA16F } } },
		Outputs { { E_CHAN_OUT::COLOR_0, { "jkb", imageRGBA16F } } },
		Programs {
			{ "range filter",
			  std::vector<FilePath> { "default.vert", "range_filter.frag" },
			  Uniforms {

				  { { "image size", E_TYPE::VEC2I, StructUniformValue<Vec2i> { Vec2i(1920,1080) } }
					 } } } }
	}; */


	enum class E_PASS : size_t
	{
		GEOMETRIC,
		DEPTH,
		//LOG_DEPTH,
		SSAO,
		SCALE,
		BLUR,
		BLUR_SMALL,
		SHADING,
		OUTLINE,
		SELECTION,
		FXAA,
		PIXELIZE,
		CRT,
		CHROMATIC_ABERRATION,
		COLORIZE,
		DEBUG,
		SSAO_LINE,
		HBAO,
		SAO,
		BMGTAO,
		SUM_IMAGES,
		//INTEGRAL_IMAGE,
		//PRECOMPUTE_TABLES,
		//INTERPOLATE_BLUR,
		//BOX_FILTER,
		COUNT
	};

	inline std::vector<Pass *> availablePasses = { &descPassGeometric,
												   &descPassDepth,
												  // &descPassLogDepth,
												   &descPassSSAO,
												   &descPassScale,
												   &descPassBlur,
												   &descPassBlurSmall,
												   &descPassShading,
												   &descPassOutline,
												   &descPassSelection,
												   &desPassFXAA,
												   &descPassPixelize,
												   &descPassCRT,
												   &descPassChromaticAberration,
												   &descPassColorize,
												   &descPassDebug,
												   &descPassSSAO_LINE,
												   &descPassHBAO,
												   &descPassSAO ,
												   &descPassBMGTAO,
												   &descPassSumImages//,
													//&descPassIntegralImage,
												  // &descPassPrecomputeTables,
												  // &descPassInterpolateBlur,
												  // &descPassBoxFilter
												   };
} // namespace VTX::Renderer

#endif