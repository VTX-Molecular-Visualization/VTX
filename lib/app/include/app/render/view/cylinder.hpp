#ifndef __VTX_RENDER_VIEW_MOLECULE_CYLINDER__
#define __VTX_RENDER_VIEW_MOLECULE_CYLINDER__

#include "app/render/view/base_view_3d_molecule.hpp"

namespace VTX::App::Render::View
{
	class Cylinder : public BaseView3DMolecule
	{
		VTX_VIEW

	  public:
		void render( const App::Component::Render::Camera & p_camera ) const override;

	  protected:
		Renderer::GL::Program * const _createProgram() override;
		void						  _init() override;

	  private:
		explicit Cylinder( App::Component::Chemistry::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}
	};
} // namespace VTX::App::Render::View

#endif