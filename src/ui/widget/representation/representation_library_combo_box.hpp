#ifndef __VTX_UI_WIDGET_REPRESENTATION_REPRESENTATION_LIBRARY_COMBO_BOX__
#define __VTX_UI_WIDGET_REPRESENTATION_REPRESENTATION_LIBRARY_COMBO_BOX__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "id.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
#include <QWidget>
#include <QWheelEvent>

namespace VTX::UI::Widget::Representation
{
	class RepresentationLibraryComboBox : public BaseManualWidget<CustomWidget::QComboBoxMultiField>
	{
		VTX_WIDGET

	  private:
		inline static int ID_VIEW_COUNTER = 0;

	  public:
		~RepresentationLibraryComboBox();
		void localize() override;

	  protected:
		RepresentationLibraryComboBox( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		 void wheelEvent( QWheelEvent * event ) override;

	  private:
		void _onRepresentationLibraryChange( const Event::VTXEvent * const p_event );
		void _fillItemList();

		ID::VTX_ID _viewID;
	};
} // namespace VTX::UI::Widget::Representation
#endif
