#include "ui/qt/dialog/progress.hpp"
#include "ui/qt/application.hpp"
#include <QPushButton>

namespace VTX::UI::QT::Dialog
{

	Progress::Progress( const std::string_view p_text, std::optional<std::function<void( void )>> p_onCancel ) :
		BaseWidget<Progress, QProgressDialog>( APP_QT::getMainWindow() )
	{
		setWindowModality( Qt::WindowModal );
		setWindowFlags( Qt::Window | Qt::FramelessWindowHint );

		// Set text.
		setLabelText( p_text.data() );
		setFixedSize( 300, 100 );

		// Set options.
		setMinimumDuration( 0 );
		setAutoClose( false );
		setAutoReset( false );
		setValue( 0 );
		setMinimum( 0 );
		setMaximum( 0 );

		if ( p_onCancel.has_value() )
		{
			setCancelButton( new QPushButton( "Cancel", this ) );
			std::function<void( void )> & onCancel = p_onCancel.value();
			connect( this, &QProgressDialog::canceled, [ &onCancel ]() { onCancel(); } );
		}
		else
		{
			setCancelButton( nullptr );
		}
	}

} // namespace VTX::UI::QT::Dialog
