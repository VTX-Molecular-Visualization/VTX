#include "ui/qt/dialog/open.hpp"
#include <app/action/scene.hpp>

namespace VTX::UI::QT::Dialog
{

	Open::Open()
	{
		restore();

		// Set options.
		setDirectory( _lastOpenFolder );
		setFileMode( QFileDialog::FileMode::ExistingFiles );
		setAcceptMode( QFileDialog::AcceptOpen );

		// Connect open.
		connect(
			this,
			&QFileDialog::filesSelected,
			this,
			[ this ]( const QStringList & p_paths )
			{
				_lastOpenFolder = directory().absolutePath();
				save();
				close();

				// Open files.
				for ( const auto & path : p_paths )
				{
					App::ACTION_SYSTEM().execute<App::Action::Scene::LoadSystem>( path.toStdString() );
				}
			}
		);
	}

	void Open::save() { SETTINGS.setValue( _SETTING_KEY_FOLDER, _lastOpenFolder ); }

	void Open::restore()
	{
		if ( SETTINGS.contains( _SETTING_KEY_FOLDER ) )
		{
			_lastOpenFolder = SETTINGS.value( _SETTING_KEY_FOLDER ).toString();
		}
	}
} // namespace VTX::UI::QT::Dialog
