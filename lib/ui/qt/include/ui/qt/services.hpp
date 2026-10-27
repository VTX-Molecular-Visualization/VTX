#ifndef VTX_UI_QT_SERVICES_HPP
#define VTX_UI_QT_SERVICES_HPP

namespace VTX::UI::QT
{
	class ActionRegistry;

	namespace Model
	{
		class SystemModel;
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

	ActionRegistry & UI_ACTIONS();

	SelectionManager & SELECTION();

	Settings & SETTINGS();

	Style::StyleManager & STYLE();

	Widget::MainWindow & MAIN_WINDOW();

} // namespace VTX::UI::QT

#endif
