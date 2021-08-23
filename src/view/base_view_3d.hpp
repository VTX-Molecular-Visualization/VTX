#ifndef __VTX_BASE_VIEW_3D__
#define __VTX_BASE_VIEW_3D__

#include "base_view.hpp"
#include "generic/base_renderable.hpp"
#include "model/base_model_3d.hpp"
#include "object3d/camera.hpp"
#include "renderer/gl/program_manager.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX::View
{
	template<typename T,
			 typename = std::enable_if<std::is_base_of<Model::BaseModel3D<Buffer::BaseBufferOpenGL>, T>::value>>
	class BaseView3D : public BaseView<T>, public Generic::BaseRenderable
	{
		VTX_VIEW

	  public:
		virtual void render( const Object3D::Camera & p_camera ) const override
		{
			_program->use();

			// Update camera uniforms.
			// TO CHECK.
			/// TODO ! ^^
			/// if ( VTXApp::get().MASK )
			//{
			const Mat4f MVMatrix = p_camera.getViewMatrix() * BaseView<T>::_model->getTransform().get();
			_program->setMat4f( "u_MVMatrix", MVMatrix );
			_program->setMat4f( "u_projMatrix", p_camera.getProjectionMatrix() );
			_program->setMat4f( "u_normalMatrix", Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) );
			//}
		}

		virtual void init() override
		{
			// Create program.
			_program = _createProgram();
			assert( _program != nullptr );

			_init();
		}

	  protected:
		Renderer::GL::Program * _program = nullptr;

		explicit BaseView3D( T * const p_model ) : BaseView<T>( p_model ) {}
		virtual ~BaseView3D() = default;

		inline OpenGLFunctions * const _gl() const { return BaseView<T>::_model->getBuffer()->getGL(); }

		virtual Renderer::GL::Program * const _createProgram() = 0;
		virtual void						  _init() {}
	};
} // namespace VTX::View
#endif
