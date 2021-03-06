#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "representation/representation_target.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Residue;
		namespace Representation
		{
			class InstantiatedRepresentation;
		} // namespace Representation
	}	  // namespace Model

	namespace Generic
	{
		class BaseRepresentable
		{
		  public:
			using InstantiatedRepresentation = Model::Representation::InstantiatedRepresentation;
			using RepresentationTarget		 = VTX::Representation::RepresentationTarget;

			~BaseRepresentable();

			const InstantiatedRepresentation * const getRepresentation() const;
			InstantiatedRepresentation * const		 getCustomRepresentation();
			void setRepresentation( InstantiatedRepresentation * const p_representation );
			void removeRepresentation();

			bool hasCustomRepresentation() const;

			const std::map<const InstantiatedRepresentation *, RepresentationTarget> & getRepresentationData() const
			{
				return _representationTargets;
			}

			void setDefaultRepresentation();
			void setParent( BaseRepresentable * p_parent );

			void computeRepresentationTargets();
			void computeColorBuffer();

			const std::map<uint, uint> & getRepresentationAtoms(
				const InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getAtoms();
			};
			const std::map<uint, uint> & getRepresentationBonds(
				const InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getBonds();
			};
			const std::map<uint, uint> & getRepresentationRibbons(
				const InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets.at( p_representation ).getRibbons();
			};

			Model::Molecule * const getMolecule() const { return _molecule; };
			void setRepresentableMolecule( Model::Molecule * const p_molecule ) { _molecule = p_molecule; };

		  protected:
			InstantiatedRepresentation * _representation = nullptr;

			mutable std::map<const InstantiatedRepresentation *, RepresentationTarget> _representationTargets
				= std::map<const InstantiatedRepresentation *, RepresentationTarget>();

			bool _isResidueVisible( const Model::Residue & p_residue ) const;

		  private:
			Model::Molecule *		  _molecule = nullptr;
			const BaseRepresentable * _parent	= nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
