#ifndef __VTX_UI_DEBUG_PRINT_INPUTS__
#define __VTX_UI_DEBUG_PRINT_INPUTS__

#include <qnamespace.h>

namespace VTX::UI::Debug
{
	class PrintInputs
	{
	  public:
		PrintInputs();

	  private:
		void _logKeyPressed( Qt::Key p_key ) const;
		void _logKeyReleased( Qt::Key p_key ) const;
	};
} // namespace VTX::UI::Debug

#endif
