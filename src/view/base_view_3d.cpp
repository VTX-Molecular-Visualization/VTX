#include "base_view_3d.hpp"
#include "vtx_app.hpp"

namespace VTX::View
{
	template<typename T>
	void BaseView3D<T>::render()
	{
		_program->use();

		// Update camera uniforms.
		if ( VTXApp::get().MASK & VTX_MASK_CAMERA_UPDATED )
		{
			const Object3D::Camera & cam	  = VTXApp::get().getScene().getCamera();
			const Mat4f				 MVMatrix = cam.getViewMatrix() * _model->getTransform().get();
			_gl()->glUniformMatrix4fv( _uModelViewMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( MVMatrix ) );
			_gl()->glUniformMatrix4fv(
				_uProjMatrixLoc, 1, GL_FALSE, Util::Math::value_ptr( cam.getProjectionMatrix() ) );
			_gl()->glUniformMatrix4fv(
				_uNormalMatrixLoc,
				1,
				GL_FALSE,
				Util::Math::value_ptr( Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) ) );
		}

		_render();
	}

	template<typename T>
	void BaseView3D<T>::_init()
	{
		// Create program.
		_program = _createProgram( VTXApp::get().getProgramManager() );
		assert( _program != nullptr );

		// Create camera uniforms.
		_uModelViewMatrixLoc = _gl()->glGetUniformLocation( _program->getId(), "u_MVMatrix" );
		_uProjMatrixLoc		 = _gl()->glGetUniformLocation( _program->getId(), "u_projMatrix" );
		_uNormalMatrixLoc	 = _gl()->glGetUniformLocation( _program->getId(), "u_normalMatrix" );

		// Create other uniforms.
		_createUniforms();
	}

} // namespace VTX::View
