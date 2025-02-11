#include <ui/qt/settings.hpp>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT
{

	Settings::Settings() :
		QSettings( QString::fromStdString( App::Filesystem::getConfigIniFile().string() ), QSettings::IniFormat )
	{
	}

	void Settings::save()
	{
		VTX_INFO( "Saving settings: {}", fileName().toStdString() );

		for ( auto * const savable : _savables )
		{
			savable->save();
		}

		if ( status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( status() ) ) );
		}

		sync();
	}

	void Settings::restore() const
	{
		VTX_INFO( "Restoring settings: {}", fileName().toStdString() );

		if ( status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( status() ) ) );
		}

		for ( auto * const savable : _savables )
		{
			savable->restore();
		}
	}

} // namespace VTX::UI::QT
