#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/representation.hpp"
#include "model/representation/representation_enum.hpp"
#include "representation/representation_target.hpp"
#include <map>
#include <set>

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}

	namespace Generic
	{
		class BaseRepresentable
		{
		  public:
			const std::set<const Model::Representation::BaseRepresentation *> &											   getRepresentations() const;
			const std::map<const Model::Representation::BaseRepresentation *, VTX::Representation::RepresentationTarget> & getRepresentationData() const
			{
				return _representationTargets;
			}

			void addRepresentation( const Model::Representation::BaseRepresentation * p_representation ) { _representations.emplace( p_representation ); };
			void removeRepresentation( const Model::Representation::BaseRepresentation * p_representation ) { _representations.erase( p_representation ); };
			void toggleRepresentation( const Model::Representation::BaseRepresentation * p_representation )
			{
				if ( _representations.find( p_representation ) != _representations.end() )
					removeRepresentation( p_representation );
				else
					addRepresentation( p_representation );
			}

			void computeRepresentationTargets();

			const std::map<uint, uint> & getRepresentationAtoms( const Model::Representation::BaseRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getAtoms();
			};
			const std::map<uint, uint> & getRepresentationBonds( const Model::Representation::BaseRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getBonds();
			};
			const std::map<uint, uint> & getRepresentationRibbons( const Model::Representation::BaseRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getRibbons();
			};

			const Model::Molecule * const getMolecule() const { return _molecule; };

		  protected:
			std::set<const Model::Representation::BaseRepresentation *> _representations = std::set<const Model::Representation::BaseRepresentation *>();

			mutable std::map<const Model::Representation::BaseRepresentation *, VTX::Representation::RepresentationTarget> _representationTargets
				= std::map<const Model::Representation::BaseRepresentation *, VTX::Representation::RepresentationTarget>();

			void setMolecule( const Model::Molecule * const p_molecule ) { _molecule = p_molecule; };

		  private:
			const Model::Molecule * _molecule;
		};
	} // namespace Generic
} // namespace VTX
#endif
