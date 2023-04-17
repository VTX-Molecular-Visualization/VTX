#ifndef __VTX_UI_WIDGET_RENDER_EFFECT_REPRESENTATION_LIBRARY_COMBO_BOX__
#define __VTX_UI_WIDGET_RENDER_EFFECT_REPRESENTATION_LIBRARY_COMBO_BOX__

#include <app/old_app/event/event.hpp>
#include <app/old_app/id.hpp>
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/qt_multi_data_field.hpp"
#include <QIcon>
#include <QWheelEvent>
#include <QWidget>

namespace VTX::UI::Widget::Renderer
{
	class RenderEffectLibraryComboBox : public BaseManualWidget<CustomWidget::QComboBoxMultiField>
	{
		VTX_WIDGET

	  private:
		inline static int	ID_VIEW_COUNTER			  = 0;
		inline static QIcon NOT_APPLIED_ITEM_FEEDABCK = QIcon();

	  public:
		~RenderEffectLibraryComboBox();
		void localize() override;

		void setHighlightAppliedItem( const bool p_highlight );

	  protected:
		RenderEffectLibraryComboBox( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void wheelEvent( QWheelEvent * event ) override;

	  private:
		void _onLibraryChange( const VTX::Event::VTXEvent * const p_event );
		void _fillItemList();
		void _updateHighlightAppliedItemFeedback();

		const ID::VTX_ID _viewID;
		bool			 _highlightApplied = false;
	};
} // namespace VTX::UI::Widget::Renderer
#endif
