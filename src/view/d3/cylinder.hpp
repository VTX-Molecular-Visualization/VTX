#ifndef __VTX_VIEW_3D_MOLECULE_CYLINDER__
#define __VTX_VIEW_3D_MOLECULE_CYLINDER__

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
			class Cylinder : public BaseView3DMolecule
			{
				VTX_VIEW

			  protected:
				Renderer::GL::Program * const _createProgram() override;
				void						  _init() override;
				void _render( const Model::Representation::InstantiatedRepresentation * const ) override;

			  private:
				GLint _uRadiusLoc = GL_INVALID_INDEX;

				explicit Cylinder( Model::Molecule * const p_model ) : BaseView3DMolecule( p_model ) {}
			};
		} // namespace D3
	}	  // namespace View
} // namespace VTX

#endif
