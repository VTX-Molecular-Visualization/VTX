#include "ui/qt/services.hpp"
#include <app/ecs.hpp>

namespace VTX::UI::QT
{
	ActionRegistry & UI_ACTIONS() { return App::ECS::getCtx<ActionRegistry>(); }

	SelectionManager & SELECTION() { return App::ECS::getCtx<SelectionManager>(); }

	Settings & SETTINGS() { return App::ECS::getCtx<Settings>(); }

	Style::StyleManager & STYLE() { return App::ECS::getCtx<Style::StyleManager>(); }

	Widget::MainWindow & MAIN_WINDOW() { return App::ECS::getCtx<Widget::MainWindow>(); }
} // namespace VTX::UI::QT
