#ifndef __VTX_VIEW_3D_MOLECULE_SPHERE__
#define __VTX_VIEW_3D_MOLECULE_SPHERE__

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
			class Sphere : public BaseView3DMolecule
			{
				VTX_VIEW

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;
				void _render( const Model::Representation::InstantiatedRepresentation * const ) override;

			  private:
				GLint _uIsRadiusFixedLoc = GL_INVALID_INDEX;
				GLint _uRadiusAddLoc	 = GL_INVALID_INDEX;
				GLint _uRadiusFixedLoc	 = GL_INVALID_INDEX;

				explicit Sphere( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
