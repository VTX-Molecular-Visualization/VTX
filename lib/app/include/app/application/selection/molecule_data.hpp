#ifndef __VTX_APP_APPLICATION_SELECTION_MOLECULE_DATA__
#define __VTX_APP_APPLICATION_SELECTION_MOLECULE_DATA__

#include "app/application/selection/selection_data.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include <util/math/range.hpp>
#include <util/math/range_list.hpp>

namespace VTX::App::Application::Selection
{
	class MoleculeData : public SelectionData
	{
	  private:
		using IndexRange	 = Util::Math::Range<size_t>;
		using IndexRangeList = Util::Math::RangeList<size_t>;

	  public:
		MoleculeData( Component::Scene::Selectable & p_selectable );

		void selectAll();

		void selectChain( const Component::Chemistry::Chain & p_chainIndex );
		void selectChains( const std::vector<Component::Chemistry::Chain *> p_chains );
		void selectChains( const IndexRange p_range );

		const IndexRangeList & getChainIds() const { return _chainIds; }
		const IndexRangeList & getResidueIds() const { return _residueIds; }
		const IndexRangeList & getAtomIds() const { return _atomIds; }

	  private:
		Component::Chemistry::Molecule * const _molecule;

		IndexRangeList _chainIds   = IndexRangeList();
		IndexRangeList _residueIds = IndexRangeList();
		IndexRangeList _atomIds	   = IndexRangeList();
	};

} // namespace VTX::App::Application::Selection
#endif
