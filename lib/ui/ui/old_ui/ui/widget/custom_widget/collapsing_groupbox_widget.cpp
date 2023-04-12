#include "collapsing_groupbox_widget.hpp"

CollapsingGroupboxWidget::CollapsingGroupboxWidget( QWidget * p_parent )
{
	this->setStyleSheet(
		"CollapsingGroupboxWidget::indicator:unchecked { image: url(:/sprite/collapsing_groupbox_expand.png); }"
		"CollapsingGroupboxWidget::indicator:checked { image: url(:/sprite/collapsing_groupbox_collapse.png); }"
		"CollapsingGroupboxWidget::title { subcontrol-origin: margin; left: 0px; padding: 0px 5px 0px 0px;}" );

	if ( !this->isCheckable() )
	{
		this->setCheckable( true );
	}

	QObject::connect( this, SIGNAL( clicked( bool ) ), this, SLOT( toggleBodyVisibility( bool ) ) );
}

void CollapsingGroupboxWidget::toggleBodyVisibility( bool checked )
{
	if ( checked )
	{
		expand();
	}
	else
	{
		collapse();
	}
}

void CollapsingGroupboxWidget::collapse()
{
	// check if there is a widget containing all the widgets of the groupbox in order to make it
	// invisible
	QWidget * containingWidget = this->findChild<QWidget *>( "", Qt::FindDirectChildrenOnly );
	if ( containingWidget != nullptr )
	{
		containingWidget->setVisible( false );
	}
	else
	{
		// go through all the children of the groupbox
		QListIterator<QObject *> iterator( this->children() );
		while ( iterator.hasNext() )
		{
			QWidget * wid = qobject_cast<QWidget *>( iterator.next() );
			if ( wid )
			{
				wid->setVisible( false );
			}
		}
	}
	this->updateGeometry();
	this->setStyleSheet(
		"CollapsingGroupboxWidget  { border-top: 1px solid #111111; border-bottom: 0px; border-left: 0px; "
		"border-right: 0px; margin-top: 6px;}"
		"CollapsingGroupboxWidget::indicator:unchecked { image: url(:/sprite/checkbox_unchecked.png); }"
		"CollapsingGroupboxWidget::indicator:checked { image: url(:/sprite/checkbox_checked.png); }"
		"CollapsingGroupboxWidget::title { subcontrol-origin: margin; left: 0px; padding: 0px 5px 0px 0px;}" );
}

void CollapsingGroupboxWidget::expand()
{
	// check if there is a widget containing all the widgets of the groupbox in order to make it visible
	QWidget * containingWidget = this->findChild<QWidget *>( "", Qt::FindDirectChildrenOnly );
	if ( containingWidget != nullptr )
	{
		containingWidget->setVisible( true );
	}
	else
	{
		// go through all the children of the groupbox
		QListIterator<QObject *> iterator( this->children() );
		while ( iterator.hasNext() )
		{
			QWidget * wid = qobject_cast<QWidget *>( iterator.next() );
			if ( wid )
			{
				wid->setVisible( true );
			}
		}
	}
	this->updateGeometry();
	this->setStyleSheet(
		"CollapsingGroupboxWidget  { border-top: 1px solid #111111; border-bottom: 1px solid #111111; "
		"border-left: 1px solid #111111; border-right: 1px solid #111111; margin-top: 6px;}"
		"CollapsingGroupboxWidget::indicator:unchecked { image: url(:/sprite/checkbox_unchecked.png); }"
		"CollapsingGroupboxWidget::indicator:checked { image: url(:/sprite/checkbox_checked.png); }"
		"CollapsingGroupboxWidget::title { subcontrol-origin: margin; left: 0px; padding: 0px 5px 0px 0px;}" );
}
