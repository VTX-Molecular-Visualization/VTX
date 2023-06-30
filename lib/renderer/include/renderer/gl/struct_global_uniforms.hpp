#ifndef __VTX_RENDERER_GL_STRUCT_GLOBAL_UNIFORMS__
#define __VTX_RENDERER_GL_STRUCT_GLOBAL_UNIFORMS__

#include "enum_shading.hpp"
#include <util/color/rgba.hpp>

namespace VTX::Renderer::GL
{
	// TODO: split in ubo per pass?
	// TODO: model and normal matrix.
	// TODO: min/max values.
	struct StructGlobalUniforms
	{
		Mat4f matrixModel  = MAT4F_ID; // TODO: will be moved.
		Mat4f matrixNormal = MAT4F_ID; // TODO: will be moved.
		Mat4f matrixView;
		Mat4f matrixProjection;

		Vec4f cameraClipInfos; // _near * _far, _far, _far - _near, _near

		Util::Color::Rgba colorBackground = Util::Color::Rgba::BLACK;
		Util::Color::Rgba colorLight	  = Util::Color::Rgba::WHITE;
		Util::Color::Rgba colorFog		  = Util::Color::Rgba::WHITE;
		Util::Color::Rgba colorOutline	  = Util::Color::Rgba::WHITE;
		Util::Color::Rgba colorSelection  = Util::Color::Rgba::WHITE;

		float specularFactor = 0.4f;
		float fogNear		 = 30.f;
		float fogFar		 = 80.f;
		float fogDensity	 = 0.f;

		float ssaoIntensity	   = 5.f;
		float blurSize		   = 17.f;
		float outlineSensivity = 0.4f;
		float outlineThickness = 1.f;

		ENUM_SHADING shadingMode		 = ENUM_SHADING::DIFFUSE;
		bool		 isCameraPerspective = true;
		uint		 pixelSize			 = 5;
		bool		 pixelizeBackground	 = true;
	};
} // namespace VTX::Renderer::GL

#endif
