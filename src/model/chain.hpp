#ifndef __VTX_MODEL_CHAIN__
#define __VTX_MODEL_CHAIN__

#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "object3d/helper/aabb.hpp"
#include <iostream>
#include <string>

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
			static const uint		NB_COLORS = 26;
			static const Color::Rgb CHAIN_ID_COLOR_ATOM[ NB_COLORS ];
			static const Color::Rgb CHAIN_ID_COLOR_HETATM[ NB_COLORS ];
			static const Color::Rgb CHAIN_ID_UNKNOWN_COLOR;

			// inline TYPE				getType() const { return _type; }
			// inline void				setType( const TYPE p_type ) { _type = p_type; }
			inline uint				getIndex() const { return _index; };
			inline void				setIndex( const uint p_index ) { _index = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			void					setMoleculePtr( Molecule * const p_molecule );

			const std::string & getOriginalChainID() const { return _originalChainID; };
			void				setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; };
			static Color::Rgb	getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm = false );

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

			inline uint getIndexLastResidue() const { return _indexFirstResidue + _residueCount - 1; };

			// Mask BaseVisible::setVisible
			void setVisible( const bool p_visible );
			void setVisible( const bool p_visible, const bool p_notify );

			const Object3D::Helper::AABB getAABB() const;
			const Object3D::Helper::AABB getWorldAABB() const;

			void removeChildrenRepresentations() const;

		  protected:
			Chain() : BaseModel( VTX::ID::Model::MODEL_CHAIN ) {}

			void _onRepresentationChange() override;

		  private:
			// TYPE	   _type		= TYPE::STANDARD;
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
