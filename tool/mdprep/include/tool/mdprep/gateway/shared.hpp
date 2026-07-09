#ifndef __VTX_TOOL_TOOLS_MDPREP_GATEWAY_SHARED__
#define __VTX_TOOL_TOOLS_MDPREP_GATEWAY_SHARED__

#include <memory>
#include <string>
#include <vector>

namespace VTX::Tool::Mdprep::Gateway
{
	// Hold information on which item the report is about. If 'none', the report is likely to refer to an item specific
	// to the active MdEngine.
	enum class E_REPORT_CHECKED_ITEM
	{
		none,
		systemWithForceField
	};

	struct CheckReport
	{
		E_REPORT_CHECKED_ITEM itemGeneric = E_REPORT_CHECKED_ITEM::none;
		int	 itemSpecific = 0; // Item specific to the MdEngine. 0 means none. In that case, refer to the generic item.
		bool pass		  = false;
		std::string message;
		bool		dirty = false;

		bool operator==( const CheckReport & ) const noexcept = default;
	};

	/**
	 * @brief Fired on the main thread when a single preparation step is about to start.
	 * @c index refers to the job index in GromacsInstructions::jobData (0-based).
	 */
	struct PreparationStepStarted
	{
		int			index = 0;
		std::string name;
	};

	/**
	 * @brief Fired on the main thread when a single preparation step is done (successfully or not).
	 * Carries a snapshot of the gromacs output channels so the UI can display them.
	 */
	struct PreparationStepFinished
	{
		int						 index	 = 0;
		bool					 success = false;
		std::string				 stdOut;
		std::string				 stdErr;
		std::vector<std::string> errors;
	};

	/**
	 * @brief Fired on the main thread once the prepared system has been packed into its output directory.
	 * @c path points to the directory holding the ready-to-use system.
	 */
	struct SystemPacked
	{
		bool		success = false;
		std::string path;
	};

	/**
	 * @brief Used as an event for when a preparation ends.
	 */
	struct PreparationFinished
	{
		bool success = false;
	};
} // namespace VTX::Tool::Mdprep::Gateway

#endif
