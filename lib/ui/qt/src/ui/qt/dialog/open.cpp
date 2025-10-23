#include "ui/qt/dialog/open.hpp"
// #include <app/action/scene.hpp>
#include "ui/qt/settings.hpp"

namespace
{
	constexpr std::string_view _SETTING_KEY_FOLDER = "dialogOpenLastOpenFolder";
}

namespace VTX::UI::QT::Dialog
{

	Open::Open()
	{
		// Set options.
		setDirectory( SETTINGS().value( _SETTING_KEY_FOLDER, "." ).toString() );
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
				SETTINGS().setValue( _SETTING_KEY_FOLDER, directory().absolutePath() );

				accept();

				// Open files.
				for ( const auto & path : p_paths )
				{
					// App::ACTION().execute<App::Action::Scene::LoadSystem>( path.toStdString() );
				}
			}
		);
	}
} // namespace VTX::UI::QT::Dialog
