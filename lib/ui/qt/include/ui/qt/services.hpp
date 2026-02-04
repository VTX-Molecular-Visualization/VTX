#ifndef __VTX_UI_QT_SERVICES__
#define __VTX_UI_QT_SERVICES__

#include <app/ecs.hpp>

namespace VTX::UI::QT
{
	class Model;
	class SelectionModel;
	class Settings;
	class Style;

	namespace Widget
	{
		class MainWindow;
	}

	inline Settings &			SETTINGS() { return App::ECS::getCtx<Settings>(); }
	inline Style &				STYLE() { return App::ECS::getCtx<Style>(); }
	inline Widget::MainWindow & MAIN_WINDOW() { return App::ECS::getCtx<Widget::MainWindow>(); }

} // namespace VTX::UI::QT

#endif
