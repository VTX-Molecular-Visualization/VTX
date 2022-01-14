#ifndef __VTX_UI_WIDGET_CONTEXTUAL_MENU_RENDER__
#define __VTX_UI_WIDGET_CONTEXTUAL_MENU_RENDER__

#include "contextual_menu_template.hpp"
#include "object3d/scene.hpp"
#include <QAction>
#include <QMenu>

namespace VTX::UI::Widget::ContextualMenu
{
	class ContextualMenuRender : public BaseContextualMenu
	{
		VTX_WIDGET

		inline static const char * SELECTION_GRANULARITY_PROPERTY_NAME = "GRANULARITY";
		inline static const char * MEASUREMENT_MODE_PROPERTY_NAME	   = "MEASUREMENT_MODE";

	  public:
		~ContextualMenuRender();
		void localize() override;

	  protected:
		ContextualMenuRender( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _loadMoleculeAction() const;
		void _downloadMoleculeAction() const;

		void _setPickerToSelection() const;
		void _setPickerToMeasurement() const;

		void _setSelectionGranularityAction( QAction * p_action ) const;
		void _setMeasurementMode( QAction * p_action ) const;
		void _showAllMoleculesAction() const;
	};

} // namespace VTX::UI::Widget::ContextualMenu

#endif
