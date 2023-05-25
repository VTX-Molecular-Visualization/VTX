#ifndef __VTX_RENDERER_GL_STRUCT_GLOBAL_UNIFORMS__
#define __VTX_RENDERER_GL_STRUCT_GLOBAL_UNIFORMS__

#include "enum_shading.hpp"
#include <util/color/rgba.hpp>

namespace VTX::Renderer::GL
{
	// TODO: split in ubo per pass?
	// TODO: model and normal matrix.
	struct StructGlobalUniforms
	{
		Mat4f matrixView	   = MAT4F_ID;
		Mat4f matrixProjection = MAT4F_ID;
		Vec4f cameraNearFar	   = Vec4f( 0.0001f, 1e4f, 0.f, 0.f ); // near, far
		Vec4f cameraClipInfos  = Vec4f( cameraNearFar.x * cameraNearFar.y,
										cameraNearFar.y,
										cameraNearFar.y - cameraNearFar.x,
										cameraNearFar.x );
		Vec4f backgroundColor  = Util::Color::Rgba::CYAN;
		Vec4f lightColor	   = Util::Color::Rgba::WHITE;
		Vec4f fog			   = Vec4f( 30.f, 1000.f, 0.f, 0.f ); // fogNear, fogFar, fogDensity
		Vec4f fogColor		   = Util::Color::Rgba::WHITE;
		Vec4f outlineColor	   = Util::Color::Rgba::WHITE;
		Vec4f selectionColor   = Util::Color::Rgba::WHITE;
		// Compress data into vec4.
		Vec4i boolData	= { true, false, false, false };			   // isCameraPerspective
		Vec4i intData	= { 1, 0, 0, 0 };							   // outlineThickness
		Vec4u uintData	= { 5, 17, uint( ENUM_SHADING::DIFFUSE ), 0 }; // ssaoIntensity, blurSize, shadingMode
		Vec4f floatData = { 0.4f, 0.4f, 0.f, 0.f };					   // specularFactor, outlineSensivity
	};
} // namespace VTX::Renderer::GL

#endif
