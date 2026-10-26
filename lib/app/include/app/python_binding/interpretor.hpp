#ifndef VTX_APP_PYTHON_BINDING_INTERPRETOR_HPP
#define VTX_APP_PYTHON_BINDING_INTERPRETOR_HPP

#include <functional>
#include <future>
#include <memory>
#include <string>
#include <util/filesystem.hpp>

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

		struct AsyncJobResult
		{
			bool		success = false;
			std::string resultStr;
		};

		Interpretor();

		/**
		 * @brief Run input python command asynchronously.
		 * @param
		 */
		void runCommand( const std::string & );

		/**
		 * @brief Run input python command asynchronously and fill the future value with its result.
		 */
		void runCommand( const std::string &, std::shared_ptr<std::promise<AsyncJobResult>> );

		/**
		 * @brief Run python script located in the input file asynchronously.
		 */
		void runScript( const FilePath & );

		/**
		 * @brief Run python script located in the input file asynchronously and fill the future with success status
		 * @param
		 * @param  Will be true if the script was successfuly executed
		 */
		void runScript( const FilePath &, const std::shared_ptr<std::promise<AsyncJobResult>> & );

		/**
		 * @brief Query the currently loaded Python runtime version and return it as "x.y.z".
		 */
		std::string getRuntimePythonVersion();

		/**
		 * @brief Slow the response time down. Python command are not actually executed right away. They are queue up
		 * and the queue is watched every once in a while and any command found is then executed. Calling this function
		 * lengthen the time interval at which the queue is watched, effectively making VTX less energy consuming while
		 * idling.
		 */
		void slowerResponseTime() noexcept;

		/**
		 * @brief Increase execution frequency. See slowerResponseTime method for more details.
		 */
		void fasterResponseTime() noexcept;

		/**
		 * @brief Give instructions to execute code on the python thread
		 * @param
		 */
		void subscribe( InterpretorInstructionsOneShot );

	  private:
		class Impl;

		struct Del
		{
			void operator()( Impl * ) noexcept;
		};

		std::unique_ptr<Impl, Del> _impl = nullptr;
	};
} // namespace VTX::App::PythonBinding

#endif
