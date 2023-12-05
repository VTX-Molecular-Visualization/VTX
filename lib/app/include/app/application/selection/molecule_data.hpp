#ifndef __VTX_APP_APPLICATION_SELECTION_MOLECULE_DATA__
#define __VTX_APP_APPLICATION_SELECTION_MOLECULE_DATA__

#include "app/application/selection/selection_data.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include <util/concepts.hpp>
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::Application::Selection
{
	using namespace Component::Chemistry;

	class MoleculeData : public SelectionData
	{
	  public:
		using IndexRange	 = Util::Math::Range<size_t>;
		using IndexRangeList = Util::Math::RangeList<size_t>;

		enum class CurrentObjectTypeEnum : int
		{
			Molecule,
			Chain,
			Residue,
			Atom,

			None
		};

	  public:
		MoleculeData( const Component::Scene::Selectable & p_selectable );

		void set( const SelectionData & p_other ) override;

		SelectionData & add( const SelectionData & p_other ) override;
		SelectionData & remove( const SelectionData & p_other ) override;
		SelectionData & intersect( const SelectionData & p_other ) override;
		SelectionData & exclude( const SelectionData & p_other ) override;

		bool isEqualsTo( const SelectionData & p_other ) const override;
		bool isValid() const override;

		void selectAll() override;
		void clear();

		bool isFullySelected() const;

		// void selectCategory( const Category & p_category );

		void referenceChain( const Chain & p_chain );
		void selectFullChain( const Chain & p_chain );
		template<ContainerOfType<const Chain *> C>
		void referenceChains( const C & p_chains )
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( chainPtr != nullptr )
					_referenceChain( *chainPtr );
			}
		}
		template<ContainerOfType<const Chain *> C>
		void selectFullChains( const C & p_chains )
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

		void unselectChain( const size_t p_chainIndex );
		void unselectChain( const Chain & p_chain );
		void unselectChains( const IndexRange & p_chain );
		void unselectChains( const IndexRangeList & p_chain );
		void unselectChains( const std::initializer_list<const Chain *> & p_chains );
		void unselectChains( const std::initializer_list<size_t> & p_chains );
		template<ContainerOfType<const Chain *> C>
		void unselectChains( const C & p_chains )
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( chainPtr != nullptr )
					_unselectChain( *chainPtr );
			}
		}

		bool isChainSelected( const size_t & p_chainIndex ) const;
		bool isChainSelected( const Chain & p_chain ) const;
		bool isChainFullySelected( const size_t & p_chainIndex ) const;
		bool isChainFullySelected( const Chain & p_chain ) const;
		bool areChainsSelected( const IndexRange & p_chains ) const;
		bool areChainsSelected( const IndexRangeList & p_chains ) const;
		bool areChainsSelected( const std::initializer_list<const Chain *> & p_chains ) const;
		bool areChainsSelected( const std::initializer_list<size_t> & p_chains ) const;
		bool areChainsFullySelected( const IndexRange & p_chains ) const;
		bool areChainsFullySelected( const IndexRangeList & p_chains ) const;
		bool areChainsFullySelected( const std::initializer_list<const Chain *> & p_chains ) const;
		bool areChainsFullySelected( const std::initializer_list<size_t> & p_chains ) const;
		template<ContainerOfType<const Chain *> C>
		bool areChainSelected( const C & p_chains ) const
		{
			for ( const Chain * const chainPtr : p_chains )
			{
				if ( !isChainSelected( *chainPtr ) )
					return false;
			}

			return true;
		}
		template<ContainerOfType<const Chain *> C>
		bool areChainsFullySelected( const C & p_chains ) const
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
		template<ContainerOfType<const Residue *> C>
		void referenceResidues( const C & p_residues )
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
		template<ContainerOfType<const Residue *> C>
		void selectFullResidues( const C & p_residues )
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

		void unselectResidue( const size_t p_residueIndex );
		void unselectResidue( const Residue & p_residue );
		void unselectResidues( const IndexRange & p_residues );
		void unselectResidues( const IndexRangeList & p_residues );
		void unselectResidues( const std::initializer_list<const Residue *> & p_residues );
		void unselectResidues( const std::initializer_list<size_t> & p_residues );
		template<ContainerOfType<const Residue *> C>
		void unselectResidues( const C & p_residues )
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( residuePtr != nullptr )
					unselectResidue( *residuePtr );
			}
		}

		bool isResidueSelected( const size_t & p_residueIndex ) const;
		bool isResidueSelected( const Residue & p_residue ) const;
		bool isResidueFullySelected( const size_t & p_residueIndex ) const;
		bool isResidueFullySelected( const Residue & p_residue ) const;
		bool areResiduesSelected( const IndexRange & p_residues ) const;
		bool areResiduesSelected( const IndexRangeList & p_residues ) const;
		bool areResiduesSelected( const std::initializer_list<const Residue *> & p_residues ) const;
		bool areResiduesSelected( const std::initializer_list<size_t> & p_residues ) const;
		bool areResiduesFullySelected( const IndexRange & p_residues ) const;
		bool areResiduesFullySelected( const IndexRangeList & p_residues ) const;
		bool areResiduesFullySelected( const std::initializer_list<const Residue *> & p_residues ) const;
		bool areResiduesFullySelected( const std::initializer_list<size_t> & p_residues ) const;

		template<ContainerOfType<const Residue *> C>
		bool areResiduesSelected( const C & p_residues ) const
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( !isResidueSelected( *residuePtr ) )
					return false;
			}

			return true;
		}
		template<ContainerOfType<const Residue *> C>
		bool areResiduesFullySelected( const C & p_residues ) const
		{
			for ( const Residue * const residuePtr : p_residues )
			{
				if ( !isResidueFullySelected( *residuePtr ) )
					return false;
			}

			return true;
		}

		void selectAtom( const Atom & p_atom );
		template<ContainerOfType<const Residue *> C>
		void selectAtoms( const C & p_atoms )
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
		void unselectAtom( const size_t p_atomIndex );
		void unselectAtoms( const IndexRange & p_atoms );
		void unselectAtoms( const IndexRangeList & p_atoms );
		void unselectAtoms( const std::initializer_list<const Atom *> & p_atoms );
		void unselectAtoms( const std::initializer_list<size_t> & p_atoms );
		template<ContainerOfType<const Atom *> C>
		void unselectAtoms( const C & p_atoms )
		{
			for ( const Atom * const atomPtr : p_atoms )
			{
				if ( atomPtr != nullptr )
					unselectAtom( *atomPtr );
			}
		}

		bool isAtomSelected( const size_t & p_atomIndex ) const;
		bool isAtomSelected( const Atom & p_atom ) const;
		bool areAtomsSelected( const IndexRange & p_atoms ) const;
		bool areAtomsSelected( const IndexRangeList & p_atoms ) const;
		bool areAtomsSelected( const std::initializer_list<const Atom *> & p_atoms ) const;
		bool areAtomsSelected( const std::initializer_list<size_t> & p_atoms ) const;
		template<ContainerOfType<const Atom *> C>
		bool areAtomsSelected( const C & p_atoms ) const
		{
			for ( const Atom * const atomPtr : p_atoms )
			{
				if ( !isAtomSelected( *atomPtr ) )
					return false;
			}

			return true;
		}

		inline Molecule &			  getMolecule() const { return *_molecule; }
		inline const IndexRangeList & getChainIds() const { return _chainIds; }
		inline const IndexRangeList & getResidueIds() const { return _residueIds; }
		inline const IndexRangeList & getAtomIds() const { return _atomIds; }

		void setCurrentObject( const Molecule & p_molecule );
		void setCurrentObject( const Chain & p_chain );
		void setCurrentObject( const Residue & p_residue );
		void setCurrentObject( const Atom & p_atom );

		CurrentObjectTypeEnum getCurrentObjectType() const { return _currentObjectType; }

		Molecule & getCurrentObjectAsMolecule() const;
		Chain &	   getCurrentObjectAsChain() const;
		Residue &  getCurrentObjectAsResidue() const;
		Atom &	   getCurrentObjectAsAtom() const;

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

		Molecule * const	  _molecule;
		CurrentObjectTypeEnum _currentObjectType  = CurrentObjectTypeEnum::Molecule;
		size_t				  _currentObjectIndex = INVALID_INDEX;

		IndexRangeList _chainIds   = IndexRangeList();
		IndexRangeList _residueIds = IndexRangeList();
		IndexRangeList _atomIds	   = IndexRangeList();
	};

} // namespace VTX::App::Application::Selection
#endif
