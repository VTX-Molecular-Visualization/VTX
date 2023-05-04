#ifndef __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__

#include "_fwd.hpp"
#include "app/application/representation/base_representable.hpp"
#include "app/component/generic/base_colorable.hpp"
#include "app/component/generic/base_visible.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include "app/core/model/base_model.hpp"
#include "app/id.hpp"
#include "app/internal/chemdb/category.hpp"
#include <iostream>
#include <string>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = App::Internal::ChemDB;

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
		// inline CHAIN_TYPE	getType() const { return _type; }
		// inline void			setType( const CHAIN_TYPE p_type ) { _type = p_type; }
		inline uint				getIndex() const { return _index; };
		inline void				setIndex( const uint p_index ) { _index = p_index; };
		inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
		void					setMoleculePtr( Molecule * const p_molecule );

		const std::string & getOriginalChainID() const { return _originalChainID; };
		void				setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; };

		inline const std::string & getName() const { return _name; };
		inline void				   setName( const std::string & p_name )
		{
			_name = p_name;
			BaseModel::setDefaultName( &_name );
		};
		inline uint getIndexFirstResidue() const { return _indexFirstResidue; };
		inline void setIndexFirstResidue( const uint p_id ) { _indexFirstResidue = p_id; };
		inline uint getResidueCount() const { return _residueCount; };
		void		setResidueCount( const uint p_count );
		uint		getRealResidueCount() const { return _realResidueCount; };
		void		removeToResidues( const uint p_residueIndex );

		uint computeRealAtomCount() const;

		inline uint getIndexLastResidue() const { return _indexFirstResidue + _residueCount - 1; };

		// Mask BaseVisible::setVisible
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
		// CHAIN_TYPE	   _type		= CHAIN_TYPE::STANDARD;
		ChemDB::Category::TYPE _categoryEnum = ChemDB::Category::TYPE::UNKNOWN;

		uint		_index			 = 0;
		Molecule *	_moleculePtr	 = nullptr;
		std::string _originalChainID = "";

		std::string _name			   = "unknown";
		uint		_indexFirstResidue = 0;
		uint		_residueCount	   = 0;
		uint		_realResidueCount  = 0;
	};

} // namespace VTX::App::Component::Chemistry
#endif
