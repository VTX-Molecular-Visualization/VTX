#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view_3d.hpp"
#include "model/molecule.hpp"
#include <map>
#include <vector>

namespace VTX
{
	namespace View
	{
		class BaseView3DMolecule : public BaseView3D<Model::Molecule>
		{
		  public:
			explicit BaseView3DMolecule( Model::Molecule * const p_model ) : BaseView3D( p_model ) {}
			virtual ~BaseView3DMolecule() {}

			virtual inline void render() final
			{
				// Render atoms.
				for ( const std::pair<const Generic::REPRESENTATION, Model::Molecule::RepresentationStruct> & pair :
					  _getModel().getRepresentationState() )
				{
					render( pair.first, pair.second.atoms, pair.second.bonds );
				}
			}

			virtual void render( const Generic::REPRESENTATION,
								 const std::map<uint, uint> &,
								 const std::map<uint, uint> & )
				= 0;
		};
	} // namespace View
} // namespace VTX
#endif
