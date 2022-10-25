#ifndef __VTX_MODEL_CATEGORY__
#define __VTX_MODEL_CATEGORY__

#include "base_model.hpp"
#include "category_enum.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "generic/base_visible.hpp"
#include "id.hpp"
#include "object3d/helper/aabb.hpp"
#include "struct/range.hpp"
#include <iostream>
#include <string>

namespace VTX::Model
{
	class Molecule;
	class Chain;
	class Residue;
	class Category : public BaseModel, public Generic::BaseVisible, public Generic::BaseRepresentable
	{
		VTX_MODEL

	  public:
		struct Iterator
		{
			using iterator_category = std::input_iterator_tag;
			using difference_type	= std::ptrdiff_t;
			using value_type		= Model::Residue;
			using pointer			= value_type *;
			using reference			= value_type &;

		  public:
			Iterator( const Model::Category & p_linkedCategory, const CATEGORY_ENUM p_category, uint p_startIndex ) :
				_currentIndex( p_startIndex ), _linkedCategory( p_linkedCategory )
			{
				_currentResidue = _getResidueAtIndex();
			};

			reference operator*() const { return *_currentResidue; }
			pointer	  operator->() const { return _currentResidue; }

			Iterator & operator++()
			{
				_next();
				return *this;
			}

			friend bool operator==( const Iterator & p_lhs, const Iterator & p_rhs )
			{
				return &( p_lhs._linkedCategory ) == &( p_rhs._linkedCategory )
					   && p_lhs._currentIndex == p_rhs._currentIndex;
			};
			friend bool operator!=( const Iterator & p_lhs, const Iterator & p_rhs )
			{
				return &( p_lhs._linkedCategory ) != &( p_rhs._linkedCategory )
					   || p_lhs._currentIndex != p_rhs._currentIndex;
			};

		  private:
			const Model::Category & _linkedCategory;

			uint			 _currentIndex;
			Model::Residue * _currentResidue = nullptr;

			void			 _next();
			Model::Residue * _getResidueAtIndex();
		};

	  public:
		friend Iterator;

		inline void				setCategoryEnum( const CATEGORY_ENUM & p_category ) { _category = p_category; };
		inline CATEGORY_ENUM	getCategoryEnum() const { return _category; };
		void					setMoleculePtr( Molecule * const p_molecule );
		inline Molecule * const getMoleculePtr() const { return _moleculePtr; };

		inline const std::string & getName() const { return CATEGORY_ENUM_STR[ int( _category ) ]; };

		void							   addResidueRange( const uint p_from, const uint p_count );
		const std::vector<Struct::Range> & getRanges() const;
		bool							   isEmpty() const;
		std::vector<Model::Chain *>		   getChains() const;
		bool							   contains( const uint p_residueIndex ) const;

		// BaseVisible::setVisible
		void setVisible( const bool p_visible );
		void setVisible( const bool p_visible, const bool p_notify );

		Iterator begin( const CATEGORY_ENUM category ) {}
		Iterator end() {}

	  protected:
		Category() : BaseModel( VTX::ID::Model::MODEL_CATEGORY ) {}

		void _onRepresentationChange() override;
		uint _getFirstResidueIndex() const;
		uint _getLastResidueIndex() const;

	  private:
		CATEGORY_ENUM			   _category = CATEGORY_ENUM::UNKNOWN;
		std::vector<Struct::Range> _ranges	 = std::vector<Struct::Range>();
		std::set<uint>			   _chainsIds;

		Molecule * _moleculePtr = nullptr;
	}; // namespace Model

} // namespace VTX::Model
#endif
