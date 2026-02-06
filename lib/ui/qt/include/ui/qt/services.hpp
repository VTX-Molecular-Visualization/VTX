#ifndef __VTX_UI_QT_SERVICES__
#define __VTX_UI_QT_SERVICES__

#include <app/ecs.hpp>

namespace VTX::UI::QT
{
	class Model;
	class SelectionModel;
	class Settings;
	namespace Style
	{
		class StyleManager;
	}
	namespace Widget
	{
		class MainWindow;
	}

	inline Settings &			 SETTINGS() { return App::ECS::getCtx<Settings>(); }
	inline Style::StyleManager & STYLE() { return App::ECS::getCtx<Style::StyleManager>(); }
	inline Widget::MainWindow &	 MAIN_WINDOW() { return App::ECS::getCtx<Widget::MainWindow>(); }

} // namespace VTX::UI::QT

#endif
