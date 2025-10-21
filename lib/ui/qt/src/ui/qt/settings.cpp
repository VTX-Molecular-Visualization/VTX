#include "ui/qt/services.hpp"
#include <app/filesystem.hpp>
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
			savable->save( *this );
		}

		if ( status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( status() ) ) );
		}

		sync();

		if ( status() != QSettings::NoError )
		{
			throw std::runtime_error( fmt::format( "{}", Util::Enum::enumName( status() ) ) );
		}
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
			savable->restore( *this );
		}
	}

	ISavable::ISavable() { SETTINGS().add( this ); }
	ISavable::~ISavable() { SETTINGS().remove( this ); }

} // namespace VTX::UI::QT
