#ifndef __VTX_UI_ENVIRONMENT__
#define __VTX_UI_ENVIRONMENT__

#include "__new_archi/ui/core/application.hpp"
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

		void setUIApp( Core::Application * const p_uiApp ) { _ui_app = p_uiApp; }

	  private:
		Core::Application * _ui_app;
	};

} // namespace VTX::UI

#endif