#ifndef __VTX_UI_QT_MODE_VISUALIZATION__
#define __VTX_UI_QT_MODE_VISUALIZATION__

#include "app/core/mode/base_mode.hpp"

namespace VTX::App::Mode
{
	class Visualization : public Core::Mode::BaseMode
	{
	  public:
		void enter() override;
		void exit() override;

		void update( const float, const float );
	};
} // namespace VTX::App::Mode
#endif
