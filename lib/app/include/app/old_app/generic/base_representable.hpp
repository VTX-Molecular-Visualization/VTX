#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

#include "app/component/chemistry/_fwd.hpp"
#include "app/old_app/representation/representation_target.hpp"
#include <map>
#include <set>
#include <util/types.hpp>

namespace VTX
{
	namespace App::Core::Model
	{
		class BaseModel;
	}

	namespace Model
	{
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

			void initBaseRepresentable( App::Core::Model::BaseModel * const			p_model,
										Generic::BaseRepresentable * const			p_parent,
										App::Component::Chemistry::Molecule * const p_molecule );

			const InstantiatedRepresentation * const							getRepresentation() const;
			InstantiatedRepresentation * const									getCustomRepresentation();
			const std::set<Model::Representation::InstantiatedRepresentation *> getSubRepresentations() const;
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
			bool				hasParent() const;
			BaseRepresentable * getParent() const { return _parent; }
			void				setParent( BaseRepresentable * p_parent );

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

			App::Component::Chemistry::Molecule * const getMolecule() const { return _molecule; };
			void setRepresentableMolecule( App::Component::Chemistry::Molecule * const p_molecule )
			{
				_molecule = p_molecule;
			};

			App::Core::Model::BaseModel * const getLinkedModel() const { return _model; };
			void setLinkedModel( App::Core::Model::BaseModel * const p_model ) { _model = p_model; };

		  protected:
			InstantiatedRepresentation *		   _representation	   = nullptr;
			std::set<InstantiatedRepresentation *> _subRepresentations = std::set<InstantiatedRepresentation *>();

			mutable std::map<const InstantiatedRepresentation *, RepresentationTarget> _representationTargets
				= std::map<const InstantiatedRepresentation *, RepresentationTarget>();

			void		 _callRepresentationChange( const bool p_callChangeOnParents = true );
			virtual void _onRepresentationChange() {};

			void _linkRepresentationToParent() const;
			void _delinkRepresentationToParent() const;

			bool _isResidueVisible( const App::Component::Chemistry::Residue & p_residue ) const;

		  private:
			App::Component::Chemistry::Molecule * _molecule = nullptr;
			BaseRepresentable *					  _parent	= nullptr;
			App::Core::Model::BaseModel *		  _model	= nullptr;
		};
	} // namespace Generic
} // namespace VTX
#endif
