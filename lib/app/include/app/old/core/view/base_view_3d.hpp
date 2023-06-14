#ifndef __VTX_VIEW_BASE_VIEW_3D__
#define __VTX_VIEW_BASE_VIEW_3D__

#include "app/old/component/generic/base_renderable.hpp"
#include "app/old/component/render/camera.hpp"
#include "app/old/core/model/base_model_3d.hpp"
#include "app/old/render/generic/base_opengl.hpp"
#include "app/old/render/renderer/gl/program_manager.hpp"
#include "app/old/vtx_app.hpp"
#include "base_view.hpp"
#include <util/math.hpp>

namespace VTX::App::Old::Core::View
{
	template<typename T,
			 typename = std::enable_if<
				 std::is_base_of<App::Old::Core::Model::BaseModel3D<Render::Buffer::BaseBufferOpenGL>, T>::value>>
	class BaseView3D : public BaseView<T>, public Component::Generic::BaseRenderable, public Render::Generic::BaseOpenGL
	{
		VTX_VIEW

	  public:
		virtual void render( const App::Old::Component::Render::Camera & p_camera ) const override
		{
			_program->use();

			// Update camera uniforms.
			// TO CHECK.
			/// TODO ! ^^
			/// if ( App::Old::VTXApp::get().MASK )
			//{

			const Mat4f MVMatrix = p_camera.getViewMatrix() * this->_model->getTransform().get();
			_program->setMat4f( "u_MVMatrix", MVMatrix );
			_program->setMat4f( "u_projMatrix", p_camera.getProjectionMatrix() );
			_program->setMat4f( "u_normalMatrix", Util::Math::transpose( Util::Math::inverse( MVMatrix ) ) );
			//}
		}

		virtual void init() override
		{
			makeContextCurrent();
			// Create program.
			_program = _createProgram();
			assert( _program != nullptr );
			_init();
			doneContextCurrent();
		}

	  protected:
		App::Old::Render::Renderer::GL::Program * _program = nullptr;

		explicit BaseView3D( T * const p_model ) : BaseView<T>( p_model ) {}
		virtual ~BaseView3D() = default;

		virtual App::Old::Render::Renderer::GL::Program * const _createProgram() = 0;
		virtual void									   _init() {}
	};
} // namespace VTX::App::Old::Core::View
#endif
