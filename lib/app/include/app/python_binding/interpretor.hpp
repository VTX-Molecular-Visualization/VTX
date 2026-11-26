#pragma once

#include <functional>
#include <future>
#include <memory>
#include <string>
#include <util/atomic_callback.hpp>
#include <util/filesystem.hpp>
#include <vector>

namespace VTX::PythonBinding
{
	class Interpretor;
}

namespace VTX::App::PythonBinding
{
	/**
	 * @brief Class responsible for initializing the python backend and providing a facade for running python commands
	 * asynchronously.
	 */
	class Interpretor
	{
	  public:
		using InterpretorInstructionsOneShot = std::function<void( VTX::PythonBinding::Interpretor & )>;

		/**
		 * @brief Struct to hold the result of an asynchronous job.
		 */
		struct AsyncJobResult
		{
			bool		success = false;
			std::string resultStr;
		};

		/**
		 * @brief Struct to hold a completion entry.
		 */
		struct CompletionEntry
		{
			std::string scope;
			std::string name;
			std::string type;
		};

		/**
		 * @brief Constructors.
		 */
		Interpretor();
		Interpretor( const Interpretor & )			   = delete;
		Interpretor & operator=( const Interpretor & ) = delete;
		Interpretor( Interpretor && )				   = delete;
		Interpretor & operator=( Interpretor && )	   = delete;
		~Interpretor()								   = default;

		/**
		 * @brief Run input python command asynchronously.
		 */
		void runCommand( const std::string & );

		/**
		 * @brief Run input python command asynchronously and fill the future value with its result.
		 */
		void runCommand( const std::string &, const std::shared_ptr<std::promise<AsyncJobResult>> & );

		/**
		 * @brief Run python script located in the input file asynchronously.
		 */
		void runScript( const FilePath & );

		/**
		 * @brief Run python script located in the input file asynchronously and fill the promise with its result.
		 */
		void runScript( const FilePath &, const std::shared_ptr<std::promise<AsyncJobResult>> & );

		/**
		 * @brief Query the currently loaded Python runtime version as "x.y.z" and pass it to the callback, which is
		 * called on the python thread.
		 */
		void requestRuntimePythonVersion( std::function<void( std::string )> );

		/**
		 * @brief Give instructions to execute code on the python thread.
		 */
		void post( InterpretorInstructionsOneShot );

		/**
		 * @brief Request the completion table and pass it to the callback.
		 */
		void requestCompletionTable( std::function<void( std::vector<CompletionEntry> )> );

		/**
		 * @brief Set pending callback.
		 */
		void setOnPending( Util::AtomicCallback::Function, void * p_context ) noexcept;

	  private:
		class Impl;

		struct Del
		{
			void operator()( Impl * ) noexcept;
		};

		std::unique_ptr<Impl, Del> _impl = nullptr;
	};
} // namespace VTX::App::PythonBinding
