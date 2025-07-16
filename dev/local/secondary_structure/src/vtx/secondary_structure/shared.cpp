#include <optional>
#include <vtx/secondary_structure/shared.hpp>
namespace pdb100
{
	namespace
	{
		VTX::Util::DataLocker<Reporter> * g_reporterPtr = nullptr;

	}
	Reporter::~Reporter() {}
	void							  reporter( VTX::Util::DataLocker<Reporter> & _ ) noexcept { g_reporterPtr = &_; }
	VTX::Util::DataLocker<Reporter> & reporter() noexcept { return *g_reporterPtr; }

	void Reporter::add( Item p_item )
	{
		switch ( p_item.resultSummary )
		{
		case Item::ResultSummary::success: _num_success++; break;
		case Item::ResultSummary::fail: _num_failed++; break;
		case Item::ResultSummary::no_ss: _num_noSs++; break;
		}
		_items.push_back( std::move( p_item ) );
	}

} // namespace pdb100
