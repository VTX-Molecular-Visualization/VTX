#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

// TODO VTX_LOGIC Try to remove dependance from this class to model stuff
#include "model/define.hpp"
#include "model/representation/representation_target.hpp"
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
			void applyRepresentation( InstantiatedRepresentation * const p_representation,
									  const bool						 p_recompute = true,
									  const bool						 p_notify	 = true );

			virtual void removeRepresentation( const bool p_notify = true );

			bool hasCustomRepresentation() const;

			inline const std::map<const InstantiatedRepresentation *, RepresentationTarget> & getRepresentationData()
				const
			{
				return _representationTargets;
			}
			bool hasParent() const;
			void setParent( BaseRepresentable * p_parent );

			void computeAllRepresentationData();
			void computeRepresentationTargets();
			void refreshRepresentationTargets();

			inline const Representation::TargetRangeArrays & getRepresentationAtoms(
				const InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets[ p_representation ].getAtoms();
			}
			inline const Representation::TargetRangeElements & getRepresentationBonds(
				const InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets[ p_representation ].getBonds();
			}
			inline const Representation::TargetRangeElements & getRepresentationRibbons(
				const InstantiatedRepresentation * const p_representation ) const
			{
				return _representationTargets[ p_representation ].getRibbons();
			}

			Model::Molecule * const getMolecule() const { return _molecule; };
			void		 setRepresentableMolecule( Model::Molecule * const p_molecule ) { _molecule = p_molecule; };
			virtual void _onRepresentationChange() {};

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
