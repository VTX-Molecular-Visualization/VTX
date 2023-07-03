#ifndef __VTX_UI_CORE_IO_VTX_LAYOUT_READER__
#define __VTX_UI_CORE_IO_VTX_LAYOUT_READER__

#include "ui/core/layout_descriptor.hpp"
#include <vector>

namespace VTX::UI::Core::IO
{
	class VTXLayoutReader
	{
	  public:
		class Result
		{
		  public:
			LayoutDescriptor layoutDescriptor = LayoutDescriptor();
		};

		void read();

		const Result & getResult() const { return _result; }

	  private:
		Result _result;
	};

} // namespace VTX::UI::Core::IO

#endif
