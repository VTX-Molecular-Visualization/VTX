#ifndef __VTX_UI_WIDGET_RENDER_EFFECT_REPRESENTATION_LIBRARY_COMBO_BOX__
#define __VTX_UI_WIDGET_RENDER_EFFECT_REPRESENTATION_LIBRARY_COMBO_BOX__

#ifdef _MSC_VER
#pragma once
#endif

#include "event/event.hpp"
#include "id.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/qt_multi_data_field.hpp"
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
		void _onLibraryChange( const Event::VTXEvent * const p_event );
		void _fillItemList();
		void _updateHighlightAppliedItemFeedback();

		const ID::VTX_ID _viewID;
		bool			 _highlightApplied = false;
	};
} // namespace VTX::UI::Widget::Renderer
#endif
