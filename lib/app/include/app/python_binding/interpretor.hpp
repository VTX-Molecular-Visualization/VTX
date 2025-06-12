#ifndef __VTX_APP_PYTHONBINDING_INTERPRETOR__
#define __VTX_APP_PYTHONBINDING_INTERPRETOR__

#include <memory>
#include <string>

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

		void runCommand( const std::string & );

	  private:
		class _Impl;
		struct Del
		{
			void operator()( _Impl * ) noexcept;
		};
		std::unique_ptr<_Impl, Del> _impl = nullptr;
	};
} // namespace VTX::App::PythonBinding

#endif
