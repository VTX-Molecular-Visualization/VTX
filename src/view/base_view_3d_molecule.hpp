#ifndef __VTX_BASE_VIEW_3D_MOLECULE__
#define __VTX_BASE_VIEW_3D_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_view_3d.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation.hpp"
#include "representation/representation_target.hpp"
#include <map>

namespace VTX
{
	namespace View
	{
		class BaseView3DMolecule : public BaseView3D<Model::Molecule>
		{
			VTX_VIEW

		  protected:
			explicit BaseView3DMolecule( Model::Molecule * const p_model ) : BaseView3D( p_model ) {}
			virtual ~BaseView3DMolecule() {}

			inline void _render() override
			{
				// Render atoms.

				for ( const std::pair<const Model::Representation::InstantiatedRepresentation *,
									  VTX::Representation::RepresentationTarget> representationData :
					  _model->getRepresentationData() )
				{
					_render( representationData.first );
				}
			}

			virtual void _render( const Model::Representation::InstantiatedRepresentation * const ) = 0;
		};
	} // namespace View
} // namespace VTX
#endif
