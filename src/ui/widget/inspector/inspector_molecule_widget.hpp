#ifndef __VTX_UI_WIDGET_INSPECTOR_MOLECULE__
#define __VTX_UI_WIDGET_INSPECTOR_MOLECULE__

#ifdef _MSC_VER
#pragma once
#endif

#include "inspector_section.hpp"
#include "model/molecule.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
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
				class InspectorMoleculeWidget : public ViewItemWidget<Model::Molecule>
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					~InspectorMoleculeWidget();
					void refresh() override;
					void localize() override;

				  protected:
					InspectorMoleculeWidget( QWidget * p_parent ) : ViewItemWidget( p_parent ) {};

					void		 _setupUi( const QString & p_name ) override;
					virtual void _setupSlots() override;

				  private:
					CustomWidget::CollapsingHeaderWidget * _mainWidget	= nullptr;
					InspectorSection *					   _infoSection = nullptr;

					QLabel * _fullnameLabel = nullptr;
					QLabel * _nbAtomsLabel	= nullptr;

					void setModelEnableFromCheckBox( const int checkboxState ) { _model->setEnable( checkboxState > 0 ); }
				};

			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
