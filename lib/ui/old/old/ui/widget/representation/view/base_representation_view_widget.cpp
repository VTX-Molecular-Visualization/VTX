#include "ui/old_ui/ui/widget/representation/view/base_representation_view_widget.hpp"

namespace VTX::UI::Widget::Representation::View
{
	BaseRepresentationViewWidget::BaseRepresentationViewWidget( QWidget * p_parent ) :
		BaseManualWidget( p_parent ),
		TMultiDataTargetField<App::Old::Application::Representation::InstantiatedRepresentation, App::Old::Application::Representation::BaseRepresentable>()
	{
	}

	void BaseRepresentationViewWidget::_setupUi( const QString & p_name ) { BaseManualWidget::_setupUi( p_name ); }

	CustomWidget::FloatFieldSliderWidget * const BaseRepresentationViewWidget::_addFloatFieldSliderWidgetInLayout(
		const std::string & p_name,
		QLabel * const		p_label,
		const float			p_min,
		const float			p_max )
	{
		CustomWidget::FloatFieldSliderWidget * const widget
			= VTX::UI::WidgetFactory::get().instantiateWidget<CustomWidget::FloatFieldSliderWidget>( this, p_name );
		widget->setMinMax( p_min, p_max );

		_appendWidgetInLayout( p_label, widget );
		return widget;
	}

	CustomWidget::QComboBoxMultiField * const BaseRepresentationViewWidget::_addQComboBoxMultiFieldWidgetInLayout(
		const std::string &				 p_name,
		QLabel * const					 p_label,
		const std::vector<std::string> & p_values )
	{
		CustomWidget::QComboBoxMultiField * const widget = new CustomWidget::QComboBoxMultiField( this );

		QStringList values = QStringList();
		for ( const std::string & value : p_values )
		{
			values.append( QString::fromStdString( value ) );
		}
		widget->addItems( values );

		_appendWidgetInLayout( p_label, widget );
		return widget;
	}

	void BaseRepresentationViewWidget::_appendWidgetInLayout( QWidget * const p_label, QWidget * const p_widget )
	{
		QGridLayout * const layout = dynamic_cast<QGridLayout *>( parentWidget()->layout() );
		const int			row	   = layout->rowCount();
		layout->addWidget( p_label, row, 0 );
		layout->addWidget( p_widget, row, 1 );
	}

	void BaseRepresentationViewWidget::_updateLabelOverriddenProperty( QLabel * const p_label, const bool p_value )
	{
		if ( p_label->property( Style::WidgetProperty::OVERIDDEN_PARAMETER ).toBool() && p_value == false )
		{
			Util::UI::setDynamicProperty( p_label, Style::WidgetProperty::OVERIDDEN_PARAMETER, false );
		}
	}

} // namespace VTX::UI::Widget::Representation::View
