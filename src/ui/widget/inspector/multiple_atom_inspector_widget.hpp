#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__

#include "model/atom.hpp"
#include "ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/multiple_model_inspector_widget.hpp"
#include "view/base_view.hpp"
#include <QWidget>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	class MultipleAtomWidget : public MultipleModelInspectorWidget<Model::Atom>
	{
		VTX_WIDGET

	  public:
		~MultipleAtomWidget();

		void localize() override;

	  protected:
		MultipleAtomWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

		void _endOfFrameRefresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

	  private:
		InspectorSectionVLayout *		 _infoSection	= nullptr;
		CustomWidget::QLabelMultiField * _nameLabel = nullptr;
		CustomWidget::QLabelMultiField * _genericNameLabel = nullptr;

		void _resetFieldStates( const SectionFlag & p_flag );
	};
} // namespace VTX::UI::Widget::Inspector

#endif
