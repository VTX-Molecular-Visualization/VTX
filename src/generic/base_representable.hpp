#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/instantiated_representation.hpp"
#include "model/representation/representation_enum.hpp"
#include "representation/representation_target.hpp"
#include <map>
#include <set>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Residue;
	} // namespace Model

	namespace Generic
	{
		class BaseRepresentable
		{
		  public:
			~BaseRepresentable();

			const std::set<const Model::Representation::InstantiatedRepresentation *> & getRepresentations() const;
			const Model::Representation::InstantiatedRepresentation * const				getRepresentation() const;

			const std::map<const Model::Representation::InstantiatedRepresentation *,
						   VTX::Representation::RepresentationTarget> &
			getRepresentationData() const
			{
				return _representationTargets;
			}

			void addRepresentation( const Model::Representation::InstantiatedRepresentation * p_representation )
			{
				_representations.emplace( p_representation );
			};
			void removeRepresentation( const Model::Representation::InstantiatedRepresentation * p_representation )
			{
				_representations.erase( p_representation );
			};

			void computeRepresentationTargets();
			void computeColorBuffer();

			const std::map<uint, uint> & getRepresentationAtoms(
				const Model::Representation::InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getAtoms();
			};
			const std::map<uint, uint> & getRepresentationBonds(
				const Model::Representation::InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getBonds();
			};
			const std::map<uint, uint> & getRepresentationRibbons(
				const Model::Representation::InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getRibbons();
			};

			Model::Molecule * const getMolecule() const { return _molecule; };

		  protected:
			std::set<const Model::Representation::InstantiatedRepresentation *> _representations
				= std::set<const Model::Representation::InstantiatedRepresentation *>();

			mutable std::map<const Model::Representation::InstantiatedRepresentation *,
							 VTX::Representation::RepresentationTarget>
				_representationTargets = std::map<const Model::Representation::InstantiatedRepresentation *,
												  VTX::Representation::RepresentationTarget>();

			bool _isResidueVisible( const Model::Residue & p_residue ) const;
			void _setRepresentableMolecule( Model::Molecule * const p_molecule ) { _molecule = p_molecule; };

		  private:
			Model::Molecule * _molecule = nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
