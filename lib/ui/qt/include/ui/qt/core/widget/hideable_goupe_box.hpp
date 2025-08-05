#ifndef __VTX_UI_QT_CORE_WIDGET_HIDEABLE_GROUP_BOX__
#define __VTX_UI_QT_CORE_WIDGET_HIDEABLE_GROUP_BOX__

#include <QGroupBox>
#include <QVBoxLayout>

namespace VTX::UI::QT::Core::Widget
{
	/**
	 * @brief QGroupBox that hide content when unselected.
	 */
	class HideableGroupBox : public QGroupBox
	{
	  public:
		explicit HideableGroupBox( QWidget * p_parent = nullptr ) : QGroupBox( p_parent ) { _setup(); }

		explicit HideableGroupBox( const QString & p_title, QWidget * p_parent = nullptr ) :
			QGroupBox( p_title, p_parent )
		{
			_setup();
		}

		void addWidget( QWidget * p_widget ) { _contentLayout->addWidget( p_widget ); }

	  private:
		QPointer<QWidget>	  _contentWidget;
		QPointer<QVBoxLayout> _contentLayout;

		void _setup()
		{
			setCheckable( true );

			_contentWidget = new QWidget( this );
			_contentLayout = new QVBoxLayout( _contentWidget );
			_contentLayout->setContentsMargins( 0, 0, 0, 0 );

			auto * mainLayout = new QVBoxLayout( this );
			mainLayout->addWidget( _contentWidget );
			mainLayout->setContentsMargins( 0, 0, 0, 0 );
			setLayout( mainLayout );

			connect( this, &QGroupBox::toggled, _contentWidget, &QWidget::setVisible );
		}
	};
} // namespace VTX::UI::QT::Core::Widget
#endif
