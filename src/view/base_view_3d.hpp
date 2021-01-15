#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view.hpp"
#include "generic/base_renderable.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/program_manager.hpp"
#include "util/math.hpp"

namespace VTX::View
{
	template<typename T>
	class BaseView3D : public BaseView<T>, public Generic::BaseRenderable
	{
	  public:
		virtual void render() override;

	  protected:
		Renderer::GL::Program * _program = nullptr;

		GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
		GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;
		GLint _uNormalMatrixLoc	   = GL_INVALID_INDEX;

		explicit BaseView3D( T * const p_model ) : BaseView( p_model ) {}
		virtual ~BaseView3D() = default;

		inline OpenGLFunctions * const _gl() { return _model->getBuffer()->gl(); }

		virtual void _init();

		virtual Renderer::GL::Program * const _createProgram( Renderer::GL::ProgramManager & ) = 0;
		virtual void						  _createUniforms() {}
		virtual void						  _render() = 0;
	};
} // namespace VTX::View
#endif
