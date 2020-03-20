#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view.hpp"
#include "generic/base_collectionable.hpp"
#include "generic/base_renderable.hpp"
#include "model/base_model.hpp"
#include "object3d/camera.hpp"
#include "renderer/glsl/program_manager.hpp"

namespace VTX
{
	namespace View
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		class BaseView3D : public BaseView<T>, public Generic::BaseRenderable
		{
		  public:
			explicit BaseView3D( T * const p_model ) : BaseView( p_model ) {}
			virtual ~BaseView3D() {}

			virtual void clean() override {}

		  protected:
			// Uniforms.
			GLint _uViewModelMatrix = GL_INVALID_INDEX;
			GLint _uProjMatrix		= GL_INVALID_INDEX;
			bool  _isActive			= false;

			virtual void _setCameraUniforms( const Object3D::Camera & p_camera )
			{
				glUniformMatrix4fv(
					_uViewModelMatrix,
					1,
					GL_FALSE,
					Util::Math::value_ptr( p_camera.getViewMatrix() * _getModel().getTransform().get() ) );
				glUniformMatrix4fv(
					_uProjMatrix, 1, GL_FALSE, Util::Math::value_ptr( p_camera.getProjectionMatrix() ) );
			}
		};
	} // namespace View
} // namespace VTX
#endif
