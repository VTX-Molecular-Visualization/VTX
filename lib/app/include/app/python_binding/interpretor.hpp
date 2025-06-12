#ifndef __VTX_APP_PYTHONBINDING_INTERPRETOR__
#define __VTX_APP_PYTHONBINDING_INTERPRETOR__

#include <memory>
#include <string>
#include <util/singleton.hpp>

namespace VTX::App::PythonBinding
{
	/**
	 * @brief Class responsible for initializing the python backend and providing a facade for running python commands
	 * asynchronously.
	 */
	class Interpretor
	{
	  public:
		Interpretor();

		/**
		 * @brief Run input python command. Doesn't block the thread while command is running
		 * @param
		 */
		void runCommand( const std::string & ) noexcept;

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
		 * @brief Provide with the actual non-threaded interpretor
		 * BUT MAYBE THIS DESIGN IS SHIT. Should subscribe callbacks to actually execute bindings on the python threads.
		 * @param
		 */
		void get( VTX::PythonBinding::Interpretor ** ) noexcept;

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
