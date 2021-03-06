#ifndef __VTX_VIEW_UI_WIDGET_RESIDUE_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_RESIDUE_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/residue.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/inspector/inspector_item_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/inspector/inspector_section_flag.hpp"
#include "view/base_view.hpp"
#include "view/ui/widget/representation/instantiated_representation_inspector_view.hpp"
#include <QWidget>

namespace VTX::View::UI::Widget
{
	class ResidueInspectorView : public BaseView<Model::Residue>, public VTX::UI::Widget::Inspector::InspectorItemWidget
	{
		VTX_WIDGET
		VTX_VIEW

		using CollapsingHeaderWidget  = VTX::UI::Widget::CustomWidget::CollapsingHeaderWidget;
		using InspectorSection		  = VTX::UI::Widget::Inspector::InspectorSection;
		using InspectorSectionVLayout = VTX::UI::Widget::Inspector::InspectorSectionVLayout;
		using SectionFlag			  = VTX::UI::Widget::Inspector::SectionFlag;

	  public:
		~ResidueInspectorView();
		void notify( const Event::VTXEvent * const p_event ) override;

		void localize() override;

	  protected:
		ResidueInspectorView( Model::Residue * const p_residue, QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;
		void		 _refresh( const SectionFlag & p_flag = SectionFlag::ALL ) override;

	  private:
		CollapsingHeaderWidget *  _mainWidget			 = nullptr;
		InspectorSection *		  _representationSection = nullptr;
		InspectorSectionVLayout * _infoSection			 = nullptr;

		Representation::ItemRepresentationInspector * _representationWidget;

		QLabel * _fullnameLabel = nullptr;
		QLabel * _nbAtomsLabel	= nullptr;

		void setModelEnableFromCheckBox( const int );
	};

} // namespace VTX::View::UI::Widget

#endif
