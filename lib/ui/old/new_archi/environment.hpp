#ifndef __VTX_UI_ENVIRONMENT__
#define __VTX_UI_ENVIRONMENT__

#include "ui/core/base_ui_application.hpp"
#include <type_traits>

namespace VTX::UI
{
	class Environment
	{
	  public:
		inline static Environment & get()
		{
			static Environment instance;
			return instance;
		}

		template<typename T>
		T * getUIApp()
		{
			return dynamic_cast<T *>( _ui_app );
		}

		void setUIApp( Core::BaseUIApplication * const p_uiApp ) { _ui_app = p_uiApp; }

	  private:
		Core::BaseUIApplication * _ui_app;
	};

} // namespace VTX::UI

#endif