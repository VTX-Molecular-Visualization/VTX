#include "inspector_item_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QVBoxLayout>
#include <QWidget>

namespace VTX::UI::Widget::Inspector
{
	void InspectorItemWidget::setExpanded( const bool p_expanded )
	{
		if ( p_expanded )
			_mainWidget->showBody();
		else
			_mainWidget->hideBody();
	}

	void InspectorItemWidget::_setupUi( const QString & p_name )
	{
		setContentsMargins( 0, 0, 0, 0 );

		QVBoxLayout * mainLayout = new QVBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->setSpacing( 1 );

		_mainWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::CollapsingHeaderWidget>( this, "inspector_item" );
		_mainWidget->setContentsMargins( 0, 0, 0, 0 );
		_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

		QWidget * mainContent = new QWidget();
		mainContent->setContentsMargins( 0, 0, 0, 0 );
		_contentLayout = new QVBoxLayout( mainContent );
		_contentLayout->setContentsMargins( 0, 0, 0, 0 );
		_contentLayout->addStretch( 1000 );

		_mainWidget->setBody( mainContent );
		mainLayout->addWidget( _mainWidget );
	}

	void InspectorItemWidget::_appendSection( InspectorSection * const p_section )
	{
		_contentLayout->insertWidget( _contentLayout->count() - 1, p_section );
	}

	void InspectorItemWidget::_freezeRefresh( const bool p_freeze ) { _refreshFreezed = p_freeze; }
	bool InspectorItemWidget::_isRefreshFreezed() const { return _refreshFreezed; }

} // namespace VTX::UI::Widget::Inspector
