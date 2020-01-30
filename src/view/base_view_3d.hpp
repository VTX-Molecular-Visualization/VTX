#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "../GL/glsl_program_manager.hpp"
#include "../model/base_model.hpp"
#include "../object3d/camera.hpp"
#include "base_view.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView3D : public BaseView<T>
		{
		  public:
			explicit BaseView3D( T * const p_model ) : BaseView( p_model )
			{
				_visible = &_isActive; // Default disabled.
			}

		  protected:
			// Uniforms.
			GLint _uViewModelMatrix = GL_INVALID_INDEX;
			GLint _uProjMatrix		= GL_INVALID_INDEX;

			virtual void _setCameraUniforms( const Object3D::Camera & p_camera )
			{
				glUniformMatrix4fv( _uViewModelMatrix,
									1,
									GL_FALSE,
									glm::value_ptr( p_camera.getViewMatrix() * _getModel().getTransform().get() ) );
				glUniformMatrix4fv( _uProjMatrix, 1, GL_FALSE, glm::value_ptr( p_camera.getProjectionMatrix() ) );
			}

		  private:
			bool _isActive = false;
		};
	} // namespace View
} // namespace VTX
#endif
