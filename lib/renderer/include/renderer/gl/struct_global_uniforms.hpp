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
		Mat4f matrixView		  = MAT4F_ID;
		Mat4f matrixProjection	  = MAT4F_ID;
		bool  isCameraPerspective = true;
		float cameraNear		  = 0.f;
		float cameraFar			  = 1e4f;
		Vec4f clipInfos			  = Vec4f( cameraNear * cameraFar, cameraFar, cameraFar - cameraNear, cameraNear );
		bool  ssao				  = true;
		uint  ssaoIntensity		  = 5;
		uint  blurSize			  = 17;
		Vec4f backgroundColor	  = Util::Color::Rgba::BLACK;
		Vec4f lightColor		  = Util::Color::Rgba::WHITE;
		uint  shadingMode		  = uint( ENUM_SHADING::DIFFUSE );
		float specularFactor	  = 0.4f;
		bool  fog				  = true;
		float fogNear			  = 30.f;
		float fogFar			  = 1000.f;
		float fogDensity		  = 0.f;
		Vec4f fogColor			  = Util::Color::Rgba::WHITE;
		bool  outline			  = true;
		Vec4f outlineColor		  = Util::Color::Rgba::WHITE;
		int	  outlineThickness	  = 1;
		float outlineSensivity	  = 0.4f;
		Vec4f selectionColor	  = Util::Color::Rgba::WHITE;
		bool  aa				  = true;
	};
} // namespace VTX::Renderer::GL

#endif
