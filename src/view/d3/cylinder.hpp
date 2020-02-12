#ifndef __VTX_VIEW_3D_MOLECULE_CYLINDER__
#define __VTX_VIEW_3D_MOLECULE_CYLINDER__

#ifdef _MSC_VER
#pragma once
#endif

#include "../base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Cylinder : public BaseView3DMolecule
			{
			  public:
				explicit Cylinder( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}

				virtual std::string getName() const override { return ID::View::D3_CYLINDER; }
				virtual void		initItem() override;
				virtual void		notify( const Event::EVENT_MODEL & ) override;

				virtual void render() override;

			  private:
				// Uniforms.
				GLint _uRadius = GL_INVALID_INDEX;
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
