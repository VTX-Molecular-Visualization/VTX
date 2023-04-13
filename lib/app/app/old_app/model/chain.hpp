#ifndef __VTX_MODEL_CHAIN__
#define __VTX_MODEL_CHAIN__

#include "base_model.hpp"
#include "category_enum.hpp"
#include "old_app/generic/base_colorable.hpp"
#include "old_app/generic/base_representable.hpp"
#include "old_app/generic/base_visible.hpp"
#include "old_app/id.hpp"
#include "old_app/object3d/helper/aabb.hpp"
#include <iostream>
#include <string>
#include <util/types.hpp>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain :
			public BaseModel,
			public Generic::BaseColorable,
			public Generic::BaseVisible,
			public Generic::BaseRepresentable
		{
			VTX_MODEL

		  public:
			enum class TYPE : int
			{
				STANDARD,
				NON_STANDARD
			};

			// one color per chain id + 1 unknown
			static const uint		 NB_COLORS = 26;
			static const Color::Rgba CHAIN_ID_COLOR_ATOM[ NB_COLORS ];
			static const Color::Rgba CHAIN_ID_COLOR_HETATM[ NB_COLORS ];
			static const Color::Rgba CHAIN_ID_UNKNOWN_COLOR;

			// inline TYPE				getType() const { return _type; }
			// inline void				setType( const TYPE p_type ) { _type = p_type; }
			inline uint				getIndex() const { return _index; };
			inline void				setIndex( const uint p_index ) { _index = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			void					setMoleculePtr( Molecule * const p_molecule );

			const std::string & getOriginalChainID() const { return _originalChainID; };
			void				setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; };
			static Color::Rgba	getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm = false );

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
			const CATEGORY_ENUM & getCategoryEnum() const { return _categoryEnum; }
			void				  setCategoryEnum( const CATEGORY_ENUM & p_categoryEnum );

			// Mask BaseVisible::setVisible
			void setVisible( const bool p_visible );
			void setVisible( const bool p_visible, const bool p_notify );

			const Object3D::Helper::AABB getAABB() const;
			const Object3D::Helper::AABB getWorldAABB() const;

			void removeChildrenRepresentations();

		  protected:
			Chain() : BaseModel( VTX::ID::Model::MODEL_CHAIN ) {}

			void _onRepresentationChange() override;

		  private:
			// TYPE	   _type		= TYPE::STANDARD;
			CATEGORY_ENUM _categoryEnum = CATEGORY_ENUM::UNKNOWN;

			uint		_index			 = 0;
			Molecule *	_moleculePtr	 = nullptr;
			std::string _originalChainID = "";

			std::string _name			   = "unknown";
			uint		_indexFirstResidue = 0;
			uint		_residueCount	   = 0;
			uint		_realResidueCount  = 0;
		}; // namespace Model

	} // namespace Model
} // namespace VTX
#endif
