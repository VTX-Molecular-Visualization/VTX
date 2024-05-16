#ifndef __VTX_TOOL_TOOLS_MDPREP__
#define __VTX_TOOL_TOOLS_MDPREP__

#include <memory>

namespace VTX::Tool::Mdprep
{
	// Designed to be the self-contained window that allow the user to use the MDprep tool
	class MainWindow
	{
	  public:
		MainWindow();

		void show() noexcept;

	  private:
		class _impl;
		struct Del
		{
			void operator()( _impl * ) noexcept;
		};
		std::unique_ptr<_impl, Del> _pimpl = nullptr;
	};
} // namespace VTX::Tool::Mdprep
#endif
