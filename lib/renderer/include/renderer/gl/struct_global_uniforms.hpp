#ifndef __VTX_RENDERER_GL_STRUCT_GLOBAL_UNIFORMS__
#define __VTX_RENDERER_GL_STRUCT_GLOBAL_UNIFORMS__

namespace VTX::Renderer::GL
{
	// TODO: split in ubo per pass?
	// TODO: model and normal matrix.
	struct StructGlobalUniforms
	{
		Mat4f matrixView		  = MAT4F_ID;
		Mat4f matrixProjection	  = MAT4F_ID;
		bool  isCameraPerspective = true;
		float cameraNear		  = 0.00001f;
		float cameraFar			  = 1000;

		uint  ssaoIntensity;
		uint  blurSize;
		Vec2i blurDirection;

		Vec4f backgroundColor;
		float fogNear;
		float fogFar;
		float fogDensity;
		Vec4f fogColor;
		Vec4f lightColor;

		Vec4f outlineColor;
		int	  outlineThickness;
		float outlineSensivity;

		Vec4f selectionColor;
	};
} // namespace VTX::Renderer::GL

#endif
