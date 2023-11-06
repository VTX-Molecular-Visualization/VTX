#ifndef __VTX_APP_APPLICATION_SELECTION_MOLECULE_DATA__
#define __VTX_APP_APPLICATION_SELECTION_MOLECULE_DATA__

#include "app/application/selection/selection_data.hpp"
#include "app/component/chemistry/_fwd.hpp"
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

	  public:
		MoleculeData( const Component::Scene::Selectable & p_selectable );

		SelectionData & add( const SelectionData & p_other ) override;
		SelectionData & remove( const SelectionData & p_other ) override;
		SelectionData & intersect( const SelectionData & p_other ) override;
		SelectionData & exclude( const SelectionData & p_other ) override;

		void selectAll();

		// void selectCategory( const Category & p_category );

		void referenceChain( const Chain & p_chain );
		void selectFullChain( const Chain & p_chain );
		void referenceChains( const std::vector<Chain *> & p_chains );
		void selectFullChains( const std::vector<Chain *> & p_chains );
		void referenceChains( const IndexRange & p_range );
		void selectFullChains( const IndexRange & p_range );

		void referenceResidue( const Residue & p_residue );
		void selectFullResidue( const Residue & p_residue );
		void referenceResidues( const std::vector<Residue *> & p_residues );
		void selectFullResidues( const std::vector<Residue *> & p_residues );
		void referenceResidues( const IndexRange & p_range );
		void selectFullResidues( const IndexRange & p_range );

		void selectAtom( const Atom & p_atom );
		void selectAtoms( const std::vector<Atom *> & p_atoms );
		void selectAtoms( const IndexRange & p_range );

		inline Molecule &			  getMolecule() const { return *_molecule; }
		inline const IndexRangeList & getChainIds() const { return _chainIds; }
		inline const IndexRangeList & getResidueIds() const { return _residueIds; }
		inline const IndexRangeList & getAtomIds() const { return _atomIds; }

		std::string toString() const override;

	  protected:
		std::unique_ptr<SelectionData> _cloneImpl() const override;

	  private:
		void _referenceChain( const Chain & p_chain );
		void _selectFullChain( const Chain & p_chain );
		void _referenceChains( const IndexRange & p_range );
		void _selectFullChains( const IndexRange & p_range );

		void _referenceResidue( const Residue & p_residue );
		void _selectFullResidue( const Residue & p_residue );
		void _referenceResidues( const IndexRange & p_range );
		void _selectFullResidues( const IndexRange & p_range );

		void _referenceAtom( const Atom & p_atom );
		void _referenceAtoms( const IndexRange & p_range );

		Molecule * const _molecule;

		IndexRangeList _chainIds   = IndexRangeList();
		IndexRangeList _residueIds = IndexRangeList();
		IndexRangeList _atomIds	   = IndexRangeList();
	};

} // namespace VTX::App::Application::Selection
#endif
