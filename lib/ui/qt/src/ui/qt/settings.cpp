#include <app/filesystem.hpp>
#include <ui/qt/settings.hpp>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{

	Settings::Settings() :
		QSettings( QString::fromStdString( App::Filesystem::getConfigIniFile().string() ), QSettings::IniFormat )
	{
		VTX_DEBUG( "Loading settings: {}", fileName().toStdString() );
	}

	void Settings::save()
	{
		VTX_DEBUG( "Saving settings: {}", fileName().toStdString() );

		if ( status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( status() ) ) );
		}

		sync();
	}

} // namespace VTX::UI::QT
