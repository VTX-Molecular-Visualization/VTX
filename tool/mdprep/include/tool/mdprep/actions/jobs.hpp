#ifndef __VTX_TOOL_TOOLS_MDPREP_ACTIONS_JOBS__
#define __VTX_TOOL_TOOLS_MDPREP_ACTIONS_JOBS__

#include <app/threading/base_thread.hpp>
#include <memory>
#include <string>
#include <tool/mdprep/backends/gromacs/gromacs.hpp>
#include <vector>

namespace VTX::Tool::Mdprep::Actions
{
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

	/**
	 * @brief Use current settings and currently visible system to create an out-of-the-box system for MD
	 */
	class StartPreparation
	{
	  public:
		StartPreparation();
		StartPreparation( VTX::App::Threading::ThreadData );

		void execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions );
		void wait() noexcept;

	  private:
		struct _Impl;

		struct _Del
		{
			void operator()( _Impl * ) const noexcept;
		};

		std::unique_ptr<_Impl, _Del> _impl = nullptr;
	};

	/**
	 * @brief Check the stuff that is currently visible with the current MD engine to see if the system works with it.
	 */
	class CheckSystem
	{
	  public:
		CheckSystem();
		CheckSystem( VTX::App::Threading::ThreadData );

		void execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions );
		void wait() noexcept;

	  private:
		struct _Impl;

		struct _Del
		{
			void operator()( _Impl * ) const noexcept;
		};

		std::unique_ptr<_Impl, _Del> _impl = nullptr;
	};
} // namespace VTX::Tool::Mdprep::Actions
#endif
