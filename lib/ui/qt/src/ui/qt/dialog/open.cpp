#include "ui/qt/dialog/open.hpp"
#include "ui/qt/settings.hpp"
#include <app/action/io.hpp>
#include <app/action/scene.hpp>

namespace VTX::UI::QT::Dialog
{

	Open::Open()
	{
		// Set options.
		setDirectory( SETTINGS().value( SETTING_KEY_OPEN_FOLDER, "." ).toString() );
		setFileMode( QFileDialog::FileMode::ExistingFiles );
		setAcceptMode( QFileDialog::AcceptOpen );

		// Connect open.
		connect(
			this,
			&QFileDialog::filesSelected,
			this,
			[ this ]( const QStringList & p_paths )
			{
				// Save last opened folder.
				SETTINGS().setValue( SETTING_KEY_OPEN_FOLDER, directory().absolutePath() );

				// Open files.
				for ( const auto & path : p_paths )
				{
					App::ACTION().execute<App::Action::IO::Open>( path.toStdString() );
				}
			}
		);
	}
} // namespace VTX::UI::QT::Dialog
