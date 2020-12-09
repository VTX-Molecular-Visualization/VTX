#ifndef __VTX_UI_WIDGET_INSPECTOR_REPRESENTATION__
#define __VTX_UI_WIDGET_INSPECTOR_REPRESENTATION__

#ifdef _MSC_VER
#pragma once
#endif

#include "inspector_item_field.hpp"
#include "inspector_section.hpp"
#include "model/representation/representation.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget/custom_widget/colormode_field_widget.hpp"
#include "ui/widget/custom_widget/representable_field_widget.hpp"
#include "ui/widget/view_item_widget.hpp"
#include <QWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Inspector
			{
				class InspectorRepresentationWidget : public ViewItemWidget<Model::Representation::BaseRepresentation>
				{
					VTX_WIDGET

				  public:
					~InspectorRepresentationWidget();
					void refresh() override;
					void localize() override;

				  protected:
					InspectorRepresentationWidget( QWidget * p_parent ) : ViewItemWidget( p_parent ) {};

					void		 _setupUi( const QString & p_name ) override;
					virtual void _setupSlots() override;

				  private:
					CustomWidget::CollapsingHeaderWidget * _mainWidget = nullptr;

					InspectorSection * _targetSection = nullptr;

					CustomWidget::RepresentableFieldWidget * _targetsField	 = nullptr;
					CustomWidget::ColorModeFieldWidget *	 _colorModeField = nullptr;

					void setModelEnableFromCheckBox( const int checkboxState ) { _model->setEnable( checkboxState > 0 ); }

					void _onTargetChange();
					void _onColorModeChange();
				};

			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
