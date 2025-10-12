#ifndef __VTX_APP_MODE_VISUALIZATION__
#define __VTX_APP_MODE_VISUALIZATION__

#include "app/mode/base_mode.hpp"

namespace VTX::App::Mode
{
	class Visualization : public BaseMode
	{
	  public:
		void enter() override;
		void exit() override;

		void update( const float, const float );
	};
} // namespace VTX::App::Mode
#endif
