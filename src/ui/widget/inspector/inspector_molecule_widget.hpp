#ifndef __VTX_UI_WIDGET_INSPECTOR_MOLECULE__
#define __VTX_UI_WIDGET_INSPECTOR_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "inspector_section.hpp"
#include "model/molecule.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/custom_widget/transform_widget.hpp"
#include "ui/widget/view_item_widget.hpp"
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	class InspectorMoleculeWidget : public ViewItemWidget<Model::Molecule>
	{
		VTX_WIDGET

	  public:
		~InspectorMoleculeWidget();
		void refresh() override;
		void localize() override;

	  protected:
		InspectorMoleculeWidget( QWidget * p_parent ) : ViewItemWidget( p_parent ) {};

		void		 _setupUi( const QString & p_name ) override;
		virtual void _setupSlots() override;

	  private:
		CustomWidget::CollapsingHeaderWidget * _mainWidget		 = nullptr;
		InspectorSection *					   _infoSection		 = nullptr;
		InspectorSection *					   _transformSection = nullptr;

		CustomWidget::TransformWidget * _transformWidget = nullptr;
		QLabel *						_fullnameLabel	 = nullptr;
		QLabel *						_nbChainsLabel	 = nullptr;
		QLabel *						_nbResiduesLabel = nullptr;
		QLabel *						_nbAtomsLabel	 = nullptr;

		void _onTransformChange( const Math::Transform & );
		void setModelEnableFromCheckBox( const int );
	};

} // namespace VTX::UI::Widget::Inspector

#endif
