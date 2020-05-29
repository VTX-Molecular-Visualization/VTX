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
				for ( const std::pair<const Generic::REPRESENTATION, std::map<uint, uint>> & pair :
					  _getModel().getRepresentationState() )
				{
					render( pair.first, pair.second );
				}
			}

			// TODO.
			virtual void render( const Generic::REPRESENTATION, const std::map<uint, uint> & ) = 0;
		};
	} // namespace View
} // namespace VTX
#endif
