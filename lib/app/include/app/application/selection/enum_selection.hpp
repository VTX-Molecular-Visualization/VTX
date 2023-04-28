#ifndef __VTX_APP_APPLICATION_SELECTION_SELECTION_ENUM__
#define __VTX_APP_APPLICATION_SELECTION_SELECTION_ENUM__

#include <string>
#include <vector>

namespace VTX::App::Application::Selection
{
	enum class GRANULARITY : int
	{
		ATOM = 0,
		RESIDUE,
		CHAIN,
		MOLECULE,

		COUNT
	};

	inline static std::vector<std::string> SELECTION_GRANULARITY_STR { "Atom", "Residue", "Chain", "Molecule" };

} // namespace VTX::App::Application::Selection
#endif
