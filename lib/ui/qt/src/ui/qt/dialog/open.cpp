#include "ui/qt/dialog/open.hpp"
// #include <app/action/scene.hpp>

namespace VTX::UI::QT::Dialog
{

	Open::Open()
	{
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
				close();

				// Open files.
				for ( const auto & path : p_paths )
				{
					// App::ACTION().execute<App::Action::Scene::LoadSystem>( path.toStdString() );
				}
			}
		);
	}

	void Open::save( Settings & p_settings ) { p_settings.setValue( _SETTING_KEY_FOLDER, _lastOpenFolder ); }

	void Open::restore( const Settings & p_settings )
	{
		if ( p_settings.contains( _SETTING_KEY_FOLDER ) )
		{
			_lastOpenFolder = p_settings.value( _SETTING_KEY_FOLDER ).toString();
		}
	}
} // namespace VTX::UI::QT::Dialog
