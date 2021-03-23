#ifndef __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__
#define __VTX_UI_WIDGET_INSPECTOR_MULTIPLE_ATOM__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "ui//widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/multi_data_field.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/inspector/inspector_item_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "view/base_view.hpp"
#include <QWidget>
#include <unordered_set>

namespace VTX::UI::Widget::Inspector
{
	class MultipleAtomWidget : public InspectorItemWidget
	{
		VTX_WIDGET

	  public:
		~MultipleAtomWidget();

		void localize() override;
		void refresh( const SectionFlag & p_flag = SectionFlag ::ALL ) override;

		void clearTargets();
		void addTarget( Model::Atom * const p_target, const bool p_refresh = true );
		void removeTarget( Model::Atom * const p_target, const bool p_refresh = true );

	  protected:
		MultipleAtomWidget( QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

	  private:
		std::unordered_set<Model::Atom *> _targets = std::unordered_set<Model::Atom *>();

		InspectorSectionVLayout *		 _infoSection	= nullptr;
		CustomWidget::QLabelMultiField * _fullnameLabel = nullptr;

		void _resetFieldStates( const SectionFlag & p_flag );
	};
} // namespace VTX::UI::Widget::Inspector

#endif
