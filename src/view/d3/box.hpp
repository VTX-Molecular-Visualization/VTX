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
				VTX_VIEW

			  public:
				void render( const Generic::REPRESENTATION ) override;

			  private:
				// Uniforms.
				GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
				GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;

				explicit Box( Model::Molecule * const p_model );
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
