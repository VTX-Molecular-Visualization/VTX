#ifndef __VTX_MODEL_CATEGORY__
#define __VTX_MODEL_CATEGORY__

#include "base_model.hpp"
#include "category_enum.hpp"
#include <util/types.hpp>
#include "app/old_app/generic/base_colorable.hpp"
#include "app/old_app/generic/base_representable.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/object3d/helper/aabb.hpp"
#include <string>
#include <vector>

namespace VTX::Model
{
	class Molecule;

	class Category :
		public BaseModel,
		public Generic::BaseColorable,
		public Generic::BaseVisible,
		public Generic::BaseRepresentable
	{
		VTX_MODEL

	  public:
		inline void				setCategoryEnum( const CATEGORY_ENUM & p_category ) { _category = p_category; };
		inline CATEGORY_ENUM	getCategoryEnum() const { return _category; };
		void					setMoleculePtr( Molecule * const p_molecule );
		inline Molecule * const getMoleculePtr() const { return _moleculePtr; };

		inline const std::string & getName() const { return CATEGORY_ENUM_STR[ int( _category ) ]; };

		void					  addChain( const uint p_chainIndex );
		void					  removeChain( const uint p_chainIndex );
		const std::vector<uint> & getChains() const;
		bool					  isEmpty() const;

		bool hasChain( const uint p_chainIndex ) const;
		bool hasResidue( const uint p_residueIndex ) const;
		bool hasAtom( const uint p_residueIndex ) const;

		int getChainInnerIndex( const uint p_chainIndex ) const;

		// BaseVisible::setVisible
		void setVisible( const bool p_visible );
		void setVisible( const bool p_visible, const bool p_notify );

		std::vector<uint> generateAtomIndexList() const;

		void updateVisibilityState();

		const Object3D::Helper::AABB getAABB() const;
		const Object3D::Helper::AABB getWorldAABB() const;

	  protected:
		Category() : BaseModel( VTX::ID::Model::MODEL_CATEGORY ) {}

		void _onRepresentationChange() override;

	  private:
		CATEGORY_ENUM	  _category		= CATEGORY_ENUM::UNKNOWN;
		std::vector<uint> _linkedChains = std::vector<uint>();

		Molecule * _moleculePtr = nullptr;
	};

} // namespace VTX::Model
#endif
