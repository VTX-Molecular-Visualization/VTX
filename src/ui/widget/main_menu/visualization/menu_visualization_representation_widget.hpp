#ifndef __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION__
#define __VTX_UI_WIDGET_MENU_VISUALIZATION_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "menu_visualization_representation_button_widget.hpp"
#include "model/representation/representation_library.hpp"
#include "ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include "view/base_view.hpp"
#include <QWidget>
#include <map>

namespace VTX::UI::Widget::MainMenu::Visualization
{
	class MenuVisualizationRepresentationWidget :
		public VTX::View::BaseView<Model::Representation::RepresentationLibrary>,
		public MenuToolBlockWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void localize() override;

	  protected:
		MenuVisualizationRepresentationWidget(
			Model::Representation::RepresentationLibrary * const _representationLibrary,
			QWidget *											 p_parent );
		~MenuVisualizationRepresentationWidget();
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshNames();
		void _refreshView() override;

		void notify( const Event::VTXEvent * const p_event ) override;

	  private:
		std::map<int, RepresentationPresetButton *> _buttons   = std::map<int, RepresentationPresetButton *>();
		MenuToolButtonWidget *						_addPreset = nullptr;

		void _addPresetAction();
	};
} // namespace VTX::UI::Widget::MainMenu::Visualization
#endif
