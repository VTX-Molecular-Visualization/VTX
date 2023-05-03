#ifndef __VTX_RENDER_VIEW_RIBBON__
#define __VTX_RENDER_VIEW_RIBBON__

#include "app/component/chemistry/secondary_structure.hpp"
#include "app/core/view/base_view_3d.hpp"

namespace VTX::App::Render::View
{
	class Ribbon : public App::Core::View::BaseView3D<App::Component::Chemistry::SecondaryStructure>
	{
		VTX_VIEW

	  public:
		void render( const App::Component::Render::Camera & p_camera ) const override;

	  protected:
		Renderer::GL::Program * const _createProgram() override;
		void						  _init() override;

	  private:
		explicit Ribbon( App::Component::Chemistry::SecondaryStructure * const p_model ) : BaseView3D( p_model ) {}
	};
} // namespace VTX::App::Render::View

#endif
