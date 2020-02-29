#ifndef __VTX_VIEW_3D_MOLECULE_BOX__
#define __VTX_VIEW_3D_MOLECULE_BOX__

#ifdef _MSC_VER
#pragma once
#endif

#include "view/base_view_3d_molecule.hpp"

namespace VTX
{
	namespace View
	{
		namespace D3
		{
			class Box : public BaseView3DMolecule
			{
			  public:
				explicit Box( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}

				virtual std::string getName() const override { return ID::View::D3_SPHERE; };
				virtual void		init() override;
				virtual void		notify( const Event::VTX_EVENT_MODEL & ) override;

				virtual void render() override;

			  private:
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
