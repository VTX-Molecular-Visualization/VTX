#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view_3d.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include <map>

namespace VTX
{
	namespace View
	{
		class BaseView3DMolecule : public BaseView3D<Model::Molecule>
		{
		  public:
			explicit BaseView3DMolecule( Model::Molecule * const p_model ) : BaseView3D( p_model ) {}
			virtual ~BaseView3DMolecule() {}

			inline void render() override
			{
				// Render atoms.
				for ( const std::pair<const Model::Representation::BaseRepresentation *, Model::Molecule::RepresentationStruct> & pair : _model->getRepresentationState() )
				{
					render( pair.first );
				}
			}

			virtual void render( const Model::Representation::BaseRepresentation * const ) = 0;
		};
	} // namespace View
} // namespace VTX
#endif
