#ifndef __VTX_APP_PYTHONBINDING_INTERPRETOR__
#define __VTX_APP_PYTHONBINDING_INTERPRETOR__

#include <functional>
#include <future>
#include <memory>
#include <string>
#include <util/singleton.hpp>

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

		Interpretor();

		/**
		 * @brief Run input python command. Doesn't block the thread while command is running
		 * @param
		 */
		void runCommand( const std::string & ) noexcept;

		/**
		 * @brief Run input python command and fill the future value with its result.
		 */
		void runCommand( const std::string &, std::future<std::string> & ) noexcept;

		/**
		 * @brief Run python script located in the input file.
		 */
		void runScript( const FilePath & ) noexcept;

		/**
		 * @brief Run python script located in the input file and fill the future with success status
		 * @param
		 * @param  Will be true if the script was a success
		 */
		void runScript( const FilePath &, std::future<bool> & ) noexcept;

		/**
		 * @brief Return whether the last command failed or not.
		 * @return
		 */
		bool lastCommandFailed() const;

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
		void subscribe( InterpretorInstructionsOneShot ) noexcept;

	  private:
		class _Impl;
		struct Del
		{
			void operator()( _Impl * ) noexcept;
		};
		std::unique_ptr<_Impl, Del> _impl = nullptr;
	};
} // namespace VTX::App::PythonBinding

namespace VTX
{
	inline VTX::App::PythonBinding::Interpretor & INTERPRETOR()
	{
		return Util::Singleton<VTX::App::PythonBinding::Interpretor>::get();
	}
} // namespace VTX

#endif
