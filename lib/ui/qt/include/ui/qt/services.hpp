#ifndef __VTX_UI_QT_SERVICES__
#define __VTX_UI_QT_SERVICES__

#include <app/ecs.hpp>

namespace VTX::UI::QT
{
	namespace Model
	{
		class SystemModel;
		class SystemSelectionModel;
	} // namespace Model
	class SelectionManager;
	class Settings;
	namespace Style
	{
		class StyleManager;
	}
	namespace Widget
	{
		class MainWindow;
	}

	inline SelectionManager &	 SELECTION() { return App::ECS::getCtx<SelectionManager>(); }
	inline Settings &			 SETTINGS() { return App::ECS::getCtx<Settings>(); }
	inline Style::StyleManager & STYLE() { return App::ECS::getCtx<Style::StyleManager>(); }
	inline Widget::MainWindow &	 MAIN_WINDOW() { return App::ECS::getCtx<Widget::MainWindow>(); }

} // namespace VTX::UI::QT

#endif
