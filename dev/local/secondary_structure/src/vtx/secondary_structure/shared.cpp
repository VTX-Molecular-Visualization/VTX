
#include <vtx/secondary_structure/report.hpp>
//
#include <vtx/secondary_structure/shared.hpp>
namespace pdb100
{
	namespace
	{
		VTX::Util::DataLocker<Reporter> * g_reporterPtr = nullptr;

	}
	void reporter( VTX::Util::DataLocker<Reporter> & _ ) noexcept { g_reporterPtr = &_; }

	VTX::Util::DataLocker<Reporter> & reporter() noexcept { return *g_reporterPtr; }

} // namespace pdb100
