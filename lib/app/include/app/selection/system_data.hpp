#ifndef __VTX_APP_SELECTION_MOLECULE_DATA__
#define __VTX_APP_SELECTION_MOLECULE_DATA__

#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"
#include "selection_data.hpp"
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>
#include <util/types.hpp>

namespace VTX::App::Selection
{
	using namespace Component::Chemistry;

	class SystemData : public SelectionData
	{
	  public:
		using IndexRange	 = Util::Math::Range<Index>;
		using IndexRangeList = Util::Math::RangeList<Index>;

		enum class CurrentObjectTypeEnum : int
		{
			System,
			Chain,
			Residue,
			Atom,

			None
		};

	  public:
		SystemData( const Component::Scene::Selectable & p_selectable );

		void set( const SelectionData & p_other ) override;

		SelectionData & add( const SelectionData & p_other ) override;
		SelectionData & remove( const SelectionData & p_other ) override;
		SelectionData & intersect( const SelectionData & p_other ) override;
		SelectionData & exclude( const SelectionData & p_other ) override;
		bool			contains( const SelectionData & p_other ) const override;

		bool isEqualsTo( const SelectionData & p_other ) const override;
		bool isValid() const override;

		void selectAll() override;
		void clear();

		bool isFullySelected() const;

		// void selectCategory( const Category & p_category );

		void referenceChain( const Chain & p_chain );
		void selectFullChain( const Chain & p_chain );
		void referenceChains( const std::span<const Chain *> & p_chains )
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( chainPtr != nullptr )
					_referenceChain( *chainPtr );
			}
		}
		void selectFullChains( const std::span<const Chain *> & p_chains )
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( chainPtr != nullptr )
					_selectFullChain( *chainPtr );
			}
		}
		void referenceChains( const IndexRange & p_range );
		void selectFullChains( const IndexRange & p_range );
		void referenceChains( const IndexRangeList & p_rangeList );
		void selectFullChains( const IndexRangeList & p_rangeList );

		void unselectChain( const Index p_chainIndex );
		void unselectChain( const Chain & p_chain );
		void unselectChains( const IndexRange & p_chain );
		void unselectChains( const IndexRangeList & p_chain );
		void unselectChains( const std::initializer_list<const Chain *> & p_chains );
		void unselectChains( const std::initializer_list<Index> & p_chains );
		void unselectChains( const std::span<const Chain *> & p_chains )
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( chainPtr != nullptr )
					_unselectChain( *chainPtr );
			}
		}

		bool isChainSelected( const Index & p_chainIndex ) const;
		bool isChainSelected( const Chain & p_chain ) const;
		bool isChainFullySelected( const Index & p_chainIndex ) const;
		bool isChainFullySelected( const Chain & p_chain ) const;
		bool areChainsSelected( const IndexRange & p_chains ) const;
		bool areChainsSelected( const IndexRangeList & p_chains ) const;
		bool areChainsSelected( const std::initializer_list<const Chain *> & p_chains ) const;
		bool areChainsSelected( const std::initializer_list<Index> & p_chains ) const;
		bool areChainsFullySelected( const IndexRange & p_chains ) const;
		bool areChainsFullySelected( const IndexRangeList & p_chains ) const;
		bool areChainsFullySelected( const std::initializer_list<const Chain *> & p_chains ) const;
		bool areChainsFullySelected( const std::initializer_list<Index> & p_chains ) const;
		bool areChainSelected( const std::span<const Chain *> & p_chains ) const
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( !isChainSelected( *chainPtr ) )
					return false;
			}

			return true;
		}
		bool areChainsFullySelected( const std::span<const Chain *> & p_chains ) const
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( !isChainFullySelected( *chainPtr ) )
					return false;
			}

			return true;
		}

		void referenceResidue( const Residue & p_residue );
		void selectFullResidue( const Residue & p_residue );
		void referenceResidues( const std::span<const Residue *> & p_residues )
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( residuePtr != nullptr )
				{
					_referenceChain( *residuePtr->getConstChainPtr() );
					_referenceResidue( *residuePtr );
				}
			}
		}
		void selectFullResidues( const std::span<const Residue *> & p_residues )
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( residuePtr != nullptr )
				{
					_referenceChain( *residuePtr->getConstChainPtr() );
					_selectFullResidue( *residuePtr );
				}
			}
		}
		void referenceResidues( const IndexRange & p_range );
		void selectFullResidues( const IndexRange & p_range );
		void referenceResidues( const IndexRangeList & p_rangeList );
		void selectFullResidues( const IndexRangeList & p_rangeList );

		void unselectResidue( const Index p_residueIndex );
		void unselectResidue( const Residue & p_residue );
		void unselectResidues( const IndexRange & p_residues );
		void unselectResidues( const IndexRangeList & p_residues );
		void unselectResidues( const std::initializer_list<const Residue *> & p_residues );
		void unselectResidues( const std::initializer_list<Index> & p_residues );

		void unselectResidues( const std::span<const Residue *> & p_residues )
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( residuePtr != nullptr )
					unselectResidue( *residuePtr );
			}
		}

		bool isResidueSelected( const Index & p_residueIndex ) const;
		bool isResidueSelected( const Residue & p_residue ) const;
		bool isResidueFullySelected( const Index & p_residueIndex ) const;
		bool isResidueFullySelected( const Residue & p_residue ) const;
		bool areResiduesSelected( const IndexRange & p_residues ) const;
		bool areResiduesSelected( const IndexRangeList & p_residues ) const;
		bool areResiduesSelected( const std::initializer_list<const Residue *> & p_residues ) const;
		bool areResiduesSelected( const std::initializer_list<Index> & p_residues ) const;
		bool areResiduesFullySelected( const IndexRange & p_residues ) const;
		bool areResiduesFullySelected( const IndexRangeList & p_residues ) const;
		bool areResiduesFullySelected( const std::initializer_list<const Residue *> & p_residues ) const;
		bool areResiduesFullySelected( const std::initializer_list<Index> & p_residues ) const;

		bool areResiduesSelected( const std::span<const Residue *> & p_residues ) const
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( !isResidueSelected( *residuePtr ) )
					return false;
			}

			return true;
		}

		bool areResiduesFullySelected( const std::span<const Residue *> & p_residues ) const
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( !isResidueFullySelected( *residuePtr ) )
					return false;
			}

			return true;
		}

		void selectAtom( const Atom & p_atom );
		void selectAtoms( const std::span<const Atom *> & p_atoms )
		{
			for ( const Atom * const atomPtr : p_atoms )
			{
				_referenceResidue( *atomPtr->getConstResiduePtr() );
				_referenceChain( *atomPtr->getConstChainPtr() );

				if ( atomPtr != nullptr )
					_referenceAtom( *atomPtr );
			}
		}
		void selectAtoms( const IndexRange & p_range );
		void selectAtoms( const IndexRangeList & p_rangeList );

		void unselectAtom( const Atom & p_atom );
		void unselectAtom( const Index p_atomIndex );
		void unselectAtoms( const IndexRange & p_atoms );
		void unselectAtoms( const IndexRangeList & p_atoms );
		void unselectAtoms( const std::initializer_list<const Atom *> & p_atoms );
		void unselectAtoms( const std::initializer_list<Index> & p_atoms );

		void unselectAtoms( const std::span<const Atom *> & p_atoms )
		{
			for ( const Atom * const atomPtr : p_atoms )
			{
				if ( atomPtr != nullptr )
					unselectAtom( *atomPtr );
			}
		}

		bool isAtomSelected( const Index & p_atomIndex ) const;
		bool isAtomSelected( const Atom & p_atom ) const;
		bool areAtomsSelected( const IndexRange & p_atoms ) const;
		bool areAtomsSelected( const IndexRangeList & p_atoms ) const;
		bool areAtomsSelected( const std::initializer_list<const Atom *> & p_atoms ) const;
		bool areAtomsSelected( const std::initializer_list<Index> & p_atoms ) const;

		bool areAtomsSelected( const std::span<const Atom *> & p_atoms ) const
		{
			for ( const Atom * const atomPtr : p_atoms )
			{
				if ( !isAtomSelected( *atomPtr ) )
					return false;
			}

			return true;
		}

		inline Component::Chemistry::System & getSystem() const { return *_system; }
		inline const IndexRangeList &		  getChainIds() const { return _chainIds; }
		inline const IndexRangeList &		  getResidueIds() const { return _residueIds; }
		inline const IndexRangeList &		  getAtomIds() const { return _atomIds; }

		void setCurrentObject( const Component::Chemistry::System & p_system );
		void setCurrentObject( const Chain & p_chain );
		void setCurrentObject( const Residue & p_residue );
		void setCurrentObject( const Atom & p_atom );

		CurrentObjectTypeEnum getCurrentObjectType() const { return _currentObjectType; }

		Component::Chemistry::System & getCurrentObjectAsSystem() const;
		Chain &						   getCurrentObjectAsChain() const;
		Residue &					   getCurrentObjectAsResidue() const;
		Atom &						   getCurrentObjectAsAtom() const;

		Util::Math::AABB getAABB() const override;

		std::string toString() const override;

	  protected:
		std::unique_ptr<SelectionData> _cloneImpl() const override;

	  private:
		void _referenceChain( const Chain & p_chain );
		void _selectFullChain( const Chain & p_chain );
		void _referenceChains( const IndexRange & p_range );
		void _selectFullChains( const IndexRange & p_range );
		void _unselectChain( const Chain & p_chain );
		void _unselectChains( const IndexRange & p_range );

		void _referenceResidue( const Residue & p_residue );
		void _selectFullResidue( const Residue & p_residue );
		void _referenceResidues( const IndexRange & p_range );
		void _selectFullResidues( const IndexRange & p_range );
		void _unselectResidue( const Residue & p_residue );
		void _unselectResidues( const IndexRange & p_range );

		void _referenceAtom( const Atom & p_atom );
		void _referenceAtoms( const IndexRange & p_range );
		void _unselectAtom( const Atom & p_atom );
		void _unselectAtoms( const IndexRange & p_range );

		void _refreshCurrentObject();

		void _recomputeAABB() const;

		Component::Chemistry::System * const _system;
		CurrentObjectTypeEnum				 _currentObjectType	 = CurrentObjectTypeEnum::System;
		Index								 _currentObjectIndex = INVALID_INDEX;

		IndexRangeList _chainIds   = IndexRangeList();
		IndexRangeList _residueIds = IndexRangeList();
		IndexRangeList _atomIds	   = IndexRangeList();

		mutable Util::Math::AABB _localAABB = Util::Math::AABB();
	};

} // namespace VTX::App::Selection
#endif
