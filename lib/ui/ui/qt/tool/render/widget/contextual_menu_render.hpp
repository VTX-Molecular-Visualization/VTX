#ifndef __VTX_UI_QT_TOOL_RENDER_WIDGET_CONTEXTUAL_MENU__
#define __VTX_UI_QT_TOOL_RENDER_WIDGET_CONTEXTUAL_MENU__

#include "qt/widget/contextual_menu/contextual_menu_template.hpp"
#include <old/object3d/scene.hpp>
#include <QAction>
#include <QMenu>

namespace VTX::UI::QT::Tool::Render::Widget
{
	class ContextualMenuRender : public QT::Widget::ContextualMenu::BaseContextualMenu
	{
		NEW_ARCHI_VTX_WIDGET

		inline static const char * SELECTION_GRANULARITY_PROPERTY_NAME = "GRANULARITY";
		inline static const char * MEASUREMENT_MODE_PROPERTY_NAME	   = "MEASUREMENT_MODE";

	  public:
		~ContextualMenuRender();
		void localize() override;
		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		ContextualMenuRender( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _loadMoleculeAction() const;
		void _downloadMoleculeAction() const;

		void _setPickerToSelection() const;
		// void _setPickerToMeasurement() const;

		void _setSelectionGranularityAction( QAction * p_action ) const;
		// void _setMeasurementMode( QAction * p_action ) const;
		void _showAllMoleculesAction() const;

	  private:
		QAction * _selectionModeAction;
		// QAction * _measurementModeAction;

		void _refreshPickerMode() const;
	};

} // namespace VTX::UI::QT::Tool::Render::Widget

#endif
