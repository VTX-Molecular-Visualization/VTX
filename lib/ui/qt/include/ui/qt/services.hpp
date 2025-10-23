#ifndef __VTX_UI_QT_SERVICES__
#define __VTX_UI_QT_SERVICES__

#include <app/ecs.hpp>

namespace VTX::UI::QT
{
	class Settings;

	namespace Widget
	{
		class MainWindow;
	}

	inline Widget::MainWindow & MAIN_WINDOW() { return App::ECS::getCtx<Widget::MainWindow>(); }
	inline Settings &			SETTINGS() { return App::ECS::getCtx<Settings>(); }
} // namespace VTX::UI::QT

#endif
