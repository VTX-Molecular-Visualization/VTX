#ifndef __VTX_UI_QT_SERVICES__
#define __VTX_UI_QT_SERVICES__

#include <app/ecs.hpp>

namespace VTX::UI::QT
{
	class Model;
	class Settings;

	namespace Widget
	{
		class MainWindow;
	}

	inline Widget::MainWindow & MAIN_WINDOW() { return App::ECS::getCtx<Widget::MainWindow>(); }
	inline Settings &			SETTINGS() { return App::ECS::getCtx<Settings>(); }
	inline Model &				MODEL() { return App::ECS::getCtx<Model>(); }
} // namespace VTX::UI::QT

#endif
