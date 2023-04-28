#ifndef __VTX_APP_COMPONENT_CHEMISTRY_CATEGORY__
#define __VTX_APP_COMPONENT_CHEMISTRY_CATEGORY__

#include "_fwd.hpp"
#include "app/core/model/base_model.hpp"
#include "app/internal/chemdb/category.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include "app/application/representation/base_representable.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/old_app/id.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = App::Internal::ChemDB;

	class Category :
		public App::Core::Model::BaseModel,
		public Generic::BaseColorable,
		public Generic::BaseVisible,
		public App::Application::Representation::BaseRepresentable
	{
		VTX_MODEL

	  public:
		inline void setCategoryEnum( const ChemDB::Category::TYPE & p_category ) { _category = p_category; };
		inline ChemDB::Category::TYPE getCategoryEnum() const { return _category; };
		void						  setMoleculePtr( Molecule * const p_molecule );
		inline Molecule * const		  getMoleculePtr() const { return _moleculePtr; };

		inline const std::string & getName() const { return ChemDB::Category::TYPE_STR[ int( _category ) ]; };

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

		const App::Component::Object3D::Helper::AABB getAABB() const;
		const App::Component::Object3D::Helper::AABB getWorldAABB() const;

	  protected:
		Category() : BaseModel( VTX::ID::Model::MODEL_CATEGORY ) {}

		void _onRepresentationChange() override;

	  private:
		ChemDB::Category::TYPE _category	 = ChemDB::Category::TYPE::UNKNOWN;
		std::vector<uint>	   _linkedChains = std::vector<uint>();

		Molecule * _moleculePtr = nullptr;
	};

} // namespace VTX::App::Component::Chemistry
#endif
