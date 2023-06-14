#ifndef __VTX_RENDER_VIEW_MOLECULE_SPHERE__
#define __VTX_RENDER_VIEW_MOLECULE_SPHERE__

#include "app/old/render/view/base_view_3d_molecule.hpp"

namespace VTX::App::Old::Render::View
{
	class Sphere : public BaseView3DMolecule
	{
		VTX_VIEW

	  public:
		void render( const App::Old::Component::Render::Camera & p_camera ) const override;

	  protected:
		Renderer::GL::Program * const _createProgram() override;
		void						  _init() override;

	  private:
		explicit Sphere( App::Old::Component::Chemistry::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}
	};
} // namespace VTX::App::Old::Render::View

#endif
