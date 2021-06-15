#ifndef __VTX_SELECTION_ENUM__
#define __VTX_SELECTION_ENUM__

namespace VTX
{
	namespace Selection
	{
		enum class SelectionType : int
		{
			MOLECULE = 0,
			CHAINS,
			RESIDUE,
			BOND,
			ATOM,

			COUNT
		};
	} // namespace Selection
} // namespace VTX
#endif
