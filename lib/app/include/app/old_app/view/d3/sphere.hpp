#ifndef __VTX_VIEW_3D_MOLECULE_SPHERE__
#define __VTX_VIEW_3D_MOLECULE_SPHERE__

#include "app/old_app/view/base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Sphere : public BaseView3DMolecule
			{
				VTX_VIEW

			  public:
				void render( const Object3D::Camera & p_camera ) const override;

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;

			  private:
				explicit Sphere( App::Component::Chemistry::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif
