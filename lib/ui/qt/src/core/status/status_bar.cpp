#include "core/status/status_bar.hpp"
#include <QHBoxLayout>

namespace VTX::UI::QT::Core::Status
{
	StatusBar::StatusBar( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void StatusBar::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_progressBar = new QProgressBar( this );
		_progressBar->setMinimum( 0 );
		_progressBar->setValue( 0 );
		_progressBar->setMaximum( 0 );
		_progressBar->setTextVisible( false );
		_progressBar->hide();
		this->addWidget( _progressBar );

		_infoLabel = new QLabel( this );
		_infoLabel->setText( "Ready" );
		this->addWidget( _infoLabel );
	}

	void StatusBar::_setupSlots() {}

	void StatusBar::setProgress( const bool p_visible )
	{
		if ( p_visible )
		{
			_infoLabel->setText( "Busy" );
			_progressBar->show();
		}
		else
		{
			_infoLabel->setText( "Ready" );
			_progressBar->hide();
		}
	}

	void StatusBar::setText( const QString & p_text ) { _infoLabel->setText( p_text ); }

	void StatusBar::localize() {}

} // namespace VTX::UI::QT::Core::Status
