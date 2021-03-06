#ifndef __VTX_VIEW_UI_WIDGET_MOLECULE_INSPECTOR_VIEW__
#define __VTX_VIEW_UI_WIDGET_MOLECULE_INSPECTOR_VIEW__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/custom_widget/transform_widget.hpp"
#include "ui/widget/inspector/inspector_item_widget.hpp"
#include "ui/widget/inspector/inspector_section.hpp"
#include "ui/widget/representation/representation_inspector_section.hpp"
#include "view/base_view.hpp"
#include <QWidget>

namespace VTX::View::UI::Widget
{
	class MoleculeInspectorView :
		public BaseView<Model::Molecule>,
		public VTX::UI::Widget::Inspector::InspectorItemWidget
	{
		VTX_WIDGET
		VTX_VIEW

		using TransformWidget				 = VTX::UI::Widget::CustomWidget::TransformWidget;
		using CollapsingHeaderWidget		 = VTX::UI::Widget::CustomWidget::CollapsingHeaderWidget;
		using InspectorSection				 = VTX::UI::Widget::Inspector::InspectorSection;
		using InspectorSectionVLayout		 = VTX::UI::Widget::Inspector::InspectorSectionVLayout;
		using RepresentationInspectorSection = VTX::UI::Widget::Representation::RepresentationInspectorSection;
		using SectionFlag					 = VTX::UI::Widget::Inspector::SectionFlag;

	  public:
		~MoleculeInspectorView();
		void notify( const Event::VTXEvent * const p_event ) override;

		void localize() override;

	  protected:
		MoleculeInspectorView( Model::Molecule * const p_molecule, QWidget * p_parent = nullptr );

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;
		void		 _refresh( const SectionFlag & p_flag = SectionFlag::ALL ) override;

	  private:
		CollapsingHeaderWidget *  _mainWidget			 = nullptr;
		InspectorSection *		  _transformSection		 = nullptr;
		InspectorSection *		  _representationSection = nullptr;
		InspectorSectionVLayout * _infoSection			 = nullptr;

		TransformWidget * _transformWidget = nullptr;

		RepresentationInspectorSection * _representationWidget;

		QLabel * _fullnameLabel	  = nullptr;
		QLabel * _nbChainsLabel	  = nullptr;
		QLabel * _nbResiduesLabel = nullptr;
		QLabel * _nbAtomsLabel	  = nullptr;

		void _onTransformChange( const Math::Transform & );
		void setModelEnableFromCheckBox( const int );
	};

} // namespace VTX::View::UI::Widget

#endif
