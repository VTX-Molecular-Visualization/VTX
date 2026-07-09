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
