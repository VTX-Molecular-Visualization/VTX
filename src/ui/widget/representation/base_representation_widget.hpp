#ifndef __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__
#define __VTX_UI_WIDGET_BASE_REPRESENTATION_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/instantiated_representation.hpp"
#include "ui/widget/base_manual_widget.hpp"
#include "ui/widget/custom_widget/colormode_field_widget.hpp"
#include "ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/widget/custom_widget/secondary_structure_colormode_field_widget.hpp"
#include <QWidget>

namespace VTX::View::UI::Widget::Representation
{
	class InstantiatedRepresentationView;
}

namespace VTX::UI::Widget::Representation
{
	class BaseRepresentationWidget : public VTX::UI::Widget::BaseManualWidget<QWidget>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~BaseRepresentationWidget();
		void setRepresentation( Model::Representation::InstantiatedRepresentation * const p_representation );
		void refresh();
		void notifyInstantiatedRepresentationViewDeleted();

	  signals:
		void onDataChange();

	  protected:
		BaseRepresentationWidget( QWidget * p_parent = nullptr );

		Model::Representation::InstantiatedRepresentation * _instantiatedRepresentation = nullptr;

		void		 _setupUi( const QString & p_name ) override;
		void		 _setupSlots();
		virtual void _refresh() {};

		void _addSphereWidgetInLayout( const QString & p_label, const float p_min, const float p_max );
		void _addCylinderWidgetInLayout( const QString & p_label, const float p_min, const float p_max );
		void _addColorModeInLayout( const QString & p_label );
		void _addSSColorModeInLayout( const QString & p_label );

		void _setSphereValue( const float p_value );
		void _setCylinderValue( const float p_value );

		void _refreshColorModeWidget();
		void _refreshSSColorModeWidget();

		virtual void _onSphereRadiusChange( const float p_newRadius );
		virtual void _onCylinderRadiusChange( const float p_newRadius );
		virtual void _colorModeChanged( const Generic::COLOR_MODE & p_colorMode );
		virtual void _ssColorModeChanged( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode );
		void		 _colorChanged( const Color::Rgb & p_color );
		void		 _ssColorChanged( const Color::Rgb & p_color );

	  private:
		View::UI::Widget::Representation::InstantiatedRepresentationView * _representationView = nullptr;

		QGridLayout * _layout;

		CustomWidget::FloatFieldSliderWidget *				   _sphereWidget;
		CustomWidget::FloatFieldSliderWidget *				   _cylinderWidget;
		CustomWidget::ColorModeFieldWidget *				   _colorModeWidget;
		CustomWidget::SecondaryStructureColorModeFieldWidget * _ssColorModeWidget;
	};
} // namespace VTX::UI::Widget::Representation
#endif
