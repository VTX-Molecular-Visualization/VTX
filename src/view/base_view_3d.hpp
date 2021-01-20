#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view.hpp"
#include "generic/base_renderable.hpp"
#include "renderer/gl/program_manager.hpp"
#include "util/math.hpp"

namespace VTX::View
{
	template<typename T,
			 typename = std::enable_if<std::is_base_of<Model::BaseModel3D<Buffer::BaseBufferOpenGL>, T>::value>>
	class BaseView3D : public BaseView<T>, public Generic::BaseRenderable
	{
		VTX_VIEW

	  public:
		virtual void render( const Object3D::Camera & p_camera ) override
		{
			_program->use();

			// Update camera uniforms.
			// if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
			//{
			const Mat4f MVMatrix = p_camera.getViewMatrix() * _model->getTransform().get();
			_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( MVMatrix ) );
			_gl()->glUniformMatrix4fv(
				_uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( p_camera.getProjectionMatrix() ) );
			_gl()->glUniformMatrix4fv(
				_uNormalMatrixLoc,
				1,
				GL_FALSE,
				Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) ) );
			//}

			_render();
		}

		virtual void init() override
		{
			// Create program.
			_program = _createProgram();
			assert( _program != nullptr );

			// Create camera uniforms.
			_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "u_MVMatrix" );
			_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "u_projMatrix" );
			_uNormalMatrixLoc	 = _gl()->glGetUniformLocation( _program->getId(), "u_normalMatrix" );

			_init();
		}

	  protected:
		Renderer::GL::Program * _program = nullptr;

		GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
		GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;
		GLint _uNormalMatrixLoc	   = GL_INVALID_INDEX;

		explicit BaseView3D( T * const p_model ) : BaseView( p_model ) {}
		virtual ~BaseView3D() = default;

		inline OpenGLFunctions * const _gl() { return _model->getBuffer()->gl(); }

		virtual Renderer::GL::Program * const _createProgram() = 0;
		virtual void						  _init() {}
		virtual void						  _render() = 0;
	};
} // namespace VTX::View
#endif
