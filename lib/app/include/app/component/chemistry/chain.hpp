#ifndef __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__

#include "_fwd.hpp"
#include "app/application/representation/base_representable.hpp"
#include "app/component/generic/base_colorable.hpp"
#include "app/component/generic/base_visible.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include "app/core/model/base_model.hpp"
#include "app/id.hpp"
#include <core/chemdb/category.hpp>
#include <core/struct/chain.hpp>
#include <iostream>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Chain :
		public App::Core::Model::BaseModel,
		public Component::Generic::BaseColorable,
		public Component::Generic::BaseVisible,
		public App::Application::Representation::BaseRepresentable
	{
		VTX_MODEL

	  public:
		static Util::Color::Rgba getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm = false );

	  public:
		inline VTX::Core::Struct::Chain &		getChainStruct() { return *_chainStruct; };
		inline const VTX::Core::Struct::Chain & getChainStruct() const { return *_chainStruct; };

		// inline CHAIN_TYPE	getType() const { return _chainStruct->getType(); }
		// inline void			setType( const CHAIN_TYPE p_type ) { _chainStruct->setType( p_type ); }
		inline uint				getIndex() const { return _chainStruct->getIndex(); };
		inline void				setIndex( const uint p_index ) { _chainStruct->setIndex( p_index ); };
		inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
		void					setMoleculePtr( Molecule * const p_molecule );

		const std::string & getOriginalChainID() const { return _chainStruct->getOriginalChainID(); };
		void setOriginalChainID( const std::string & p_chainId ) { _chainStruct->setOriginalChainID( p_chainId ); };

		inline const std::string & getName() const { return _chainStruct->getName(); };
		void					   setName( const std::string & p_name );
		inline uint				   getIndexFirstResidue() const { return _chainStruct->getIndexFirstResidue(); };
		inline void setIndexFirstResidue( const uint p_id ) { _chainStruct->setIndexFirstResidue( p_id ); };
		inline uint getResidueCount() const { return _chainStruct->getResidueCount(); };
		inline void setResidueCount( const uint p_count ) { _chainStruct->setResidueCount( p_count ); }
		inline uint getRealResidueCount() const { return _chainStruct->getRealResidueCount(); };
		inline void removeToResidues( const uint p_residueIndex ) { _chainStruct->removeToResidue( p_residueIndex ); }

		uint computeRealAtomCount() const { return _chainStruct->computeRealAtomCount(); }

		inline uint getIndexLastResidue() const { return _chainStruct->getIndexLastResidue(); };

		const ChemDB::Category::TYPE & getCategoryEnum() const { return _categoryEnum; }
		void						   setCategoryEnum( const ChemDB::Category::TYPE & p_categoryEnum );

		// Mask BaseVisible::setVisible
		void setVisible( const bool p_visible );
		void setVisible( const bool p_visible, const bool p_notify );

		const App::Component::Object3D::Helper::AABB getAABB() const;
		const App::Component::Object3D::Helper::AABB getWorldAABB() const;

		void removeChildrenRepresentations();

	  protected:
		Chain() : BaseModel( App::ID::Model::MODEL_CHAIN ) {}

		void _onRepresentationChange() override;

	  private:
		VTX::Core::Struct::Chain * _chainStruct = nullptr;
		Molecule *				   _moleculePtr = nullptr;

		ChemDB::Category::TYPE _categoryEnum = ChemDB::Category::TYPE::UNKNOWN;
	};

} // namespace VTX::App::Component::Chemistry
#endif
