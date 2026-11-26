#ifndef VTX_TOOL_MDPREP_ACTIONS_JOBS_HPP
#define VTX_TOOL_MDPREP_ACTIONS_JOBS_HPP

#include <memory>
#include <string>
#include <tool/mdprep/backends/gromacs/gromacs.hpp>
#include <util/thread/base_thread.hpp>
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
		StartPreparation( VTX::Util::Thread::ThreadData );

		void execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions );
		void wait();

	  private:
		struct Impl;

		struct Del
		{
			void operator()( Impl * ) const noexcept;
		};

		std::unique_ptr<Impl, Del> _impl = nullptr;
	};

	/**
	 * @brief Check the stuff that is currently visible with the current MD engine to see if the system works with it.
	 */
	class CheckSystem
	{
	  public:
		CheckSystem();
		CheckSystem( VTX::Util::Thread::ThreadData );

		void execute( VTX::Tool::Mdprep::backends::Gromacs::GromacsInstructions );
		void wait();

	  private:
		struct Impl;

		struct Del
		{
			void operator()( Impl * ) const noexcept;
		};

		std::unique_ptr<Impl, Del> _impl = nullptr;
	};
} // namespace VTX::Tool::Mdprep::Actions
#endif
