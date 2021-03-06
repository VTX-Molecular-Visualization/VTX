#ifndef __VTX_VIEW_UI_WIDGET_ATOM_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_ATOM_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/atom.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/inspector/inspector_item_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/inspector_section_flag.hpp"
#include "view/base_view.hpp"
#include <QWidget>

namespace VTX::View::UI::Widget
{
	class AtomInspectorView : public BaseView<Model::Atom>, public VTX::UI::Widget::Inspector::InspectorItemWidget
	{
		VTX_WIDGET
		VTX_VIEW

		using CollapsingHeaderWidget  = VTX::UI::Widget::CustomWidget::CollapsingHeaderWidget;
		using InspectorSection		  = VTX::UI::Widget::Inspector::InspectorSection;
		using InspectorSectionVLayout = VTX::UI::Widget::Inspector::InspectorSectionVLayout;
		using SectionFlag			  = VTX::UI::Widget::Inspector::SectionFlag;

	  public:
		~AtomInspectorView();
		void notify( const Event::VTXEvent * const p_event ) override;

		void localize() override;

	  protected:
		AtomInspectorView( Model::Atom * const p_atom, QWidget * p_parent = nullptr );

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;
		void _refresh( const SectionFlag & p_flag = SectionFlag::ALL ) override;

	  private:
		CollapsingHeaderWidget *  _mainWidget  = nullptr;
		InspectorSectionVLayout * _infoSection = nullptr;

		QLabel * _fullnameLabel = nullptr;
	};

} // namespace VTX::View::UI::Widget

#endif
