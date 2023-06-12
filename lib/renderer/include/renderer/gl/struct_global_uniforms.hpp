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
		Mat4f matrixModel  = MAT4F_ID; // TODO: will be moved.
		Mat4f matrixNormal = MAT4F_ID; // TODO: will be moved.
		Mat4f matrixView;
		Mat4f matrixProjection;
		Vec4f cameraClipInfos; // _near * _far, _far, _far - _near, _near
		Vec4f backgroundColor = Util::Color::Rgba::BLACK;
		Vec4f lightColor	  = Util::Color::Rgba::WHITE;
		Vec4f fog			  = Vec4f( 30.f, 1000.f, 0.f, 0.f ); // fogNear, fogFar, fogDensity
		Vec4f fogColor		  = Util::Color::Rgba::WHITE;
		Vec4f outlineColor	  = Util::Color::Rgba::WHITE;
		Vec4f selectionColor  = Util::Color::Rgba::WHITE;
		// Compress data into vec4.
		Vec4i boolData	= { true, false, false, false };			  // isCameraPerspective
		Vec4i intData	= { 0, 0, 0, 0 };							  // outlineThickness
		Vec4u uintData	= { 0, 0, uint( ENUM_SHADING::DIFFUSE ), 0 }; // ssaoIntensity, blurSize, shadingMode
		Vec4f floatData = { 0.4f, 0.f, 0.f, 0.f };					  // specularFactor, outlineSensivity
	};
} // namespace VTX::Renderer::GL

#endif
