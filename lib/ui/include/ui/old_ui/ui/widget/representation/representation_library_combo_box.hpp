#ifndef __VTX_UI_WIDGET_REPRESENTATION_REPRESENTATION_LIBRARY_COMBO_BOX__
#define __VTX_UI_WIDGET_REPRESENTATION_REPRESENTATION_LIBRARY_COMBO_BOX__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/qt_multi_data_field.hpp"
#include <QIcon>
#include <QWheelEvent>
#include <QWidget>
#include <app/event/vtx_event.hpp>
#include <app/old_app/id.hpp>

namespace VTX::UI::Widget::Representation
{
	class RepresentationLibraryComboBox : public BaseManualWidget<CustomWidget::QComboBoxMultiField>
	{
		VTX_WIDGET

	  private:
		inline static int	ID_VIEW_COUNTER						= 0;
		inline static QIcon NOT_DEFAULT_REPRESENTATION_FEEDABCK = QIcon();

	  public:
		~RepresentationLibraryComboBox();
		void localize() override;

		void setHighlightDefaultRepresentation( const bool p_highlight );

	  protected:
		RepresentationLibraryComboBox( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void wheelEvent( QWheelEvent * event ) override;

	  private:
		void _onRepresentationLibraryChange( const VTX::Event::VTXEvent * const p_event );
		void _fillItemList();
		void _updateHighlightDefaultRepresentationFeedback();

		const ID::VTX_ID _viewID;
		bool			 _highlightDefault = false;
	};
} // namespace VTX::UI::Widget::Representation
#endif
