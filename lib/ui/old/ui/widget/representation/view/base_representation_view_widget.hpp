#ifndef __VTX_UI_BASE_REPRESENTATION_VIEW_WIDGET__
#define __VTX_UI_BASE_REPRESENTATION_VIEW_WIDGET__

#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/float_field_slider_widget.hpp"
#include "ui/old_ui/ui/widget/custom_widget/qt_multi_data_field.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include <QGridLayout>
#include <QWidget>
#include <app/old/application/representation/instantiated_representation.hpp>

namespace VTX::UI::Widget::Representation::View
{
	class BaseRepresentationViewWidget :
		public VTX::UI::Widget::BaseManualWidget<QWidget>,
		public TMultiDataTargetField<App::Old::Application::Representation::InstantiatedRepresentation, App::Old::Application::Representation::BaseRepresentable>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		virtual void localize() override {}

	  protected:
		BaseRepresentationViewWidget( QWidget * p_parent = nullptr );

		virtual void _setupUi( const QString & p_name ) override;

		CustomWidget::FloatFieldSliderWidget * const _addFloatFieldSliderWidgetInLayout( const std::string & p_name,
																						 QLabel * const		 p_label,
																						 const float		 p_min,
																						 const float		 p_max );
		CustomWidget::QComboBoxMultiField * const	 _addQComboBoxMultiFieldWidgetInLayout(
			   const std::string &				p_name,
			   QLabel * const					p_label,
			   const std::vector<std::string> & p_values );

		void _appendWidgetInLayout( QWidget * const p_label, QWidget * const p_widget );
		void _updateLabelOverriddenProperty( QLabel * const p_label, const bool p_value );

	  private:
	};
} // namespace VTX::UI::Widget::Representation::View
#endif
