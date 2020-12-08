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

			  public:
				void render( const Generic::REPRESENTATION ) override;

			  private:
				float _radiusFixed	 = 1.f;
				float _radiusAdd	 = 0.f;
				bool  _isRadiusFixed = false;

				// Uniforms.
				GLint _uModelViewMatrixLoc = GL_INVALID_INDEX;
				GLint _uProjMatrixLoc	   = GL_INVALID_INDEX;
				GLint _uIsRadiusFixedLoc   = GL_INVALID_INDEX;
				GLint _uRadiusAddLoc	   = GL_INVALID_INDEX;
				GLint _uRadiusFixedLoc	   = GL_INVALID_INDEX;

				explicit Sphere( Model::Molecule * const p_model );
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif;
