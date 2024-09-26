#include "app/core/settings/settings_system.hpp"

namespace VTX::App::Core::Settings
{
	SettingsSystem::SettingsSystem( const SettingsSystem & p_source )
	{
		for ( const auto & pair : p_source._settings )
		{
			_settings.emplace( pair.first, pair.second->clone() );
		}
	}

	void SettingsSystem::reset()
	{
		for ( auto & pair : _settings )
		{
			pair.second->reset();
		}
	}

	bool operator==( const SettingsSystem & p_lhs, const SettingsSystem & p_rhs )
	{
		if ( p_lhs._settings.size() != p_rhs._settings.size() )
			return false;

		for ( const auto & lhsPair : p_lhs._settings )
		{
			if ( !p_rhs._settings.contains( lhsPair.first ) || *p_rhs._settings.at( lhsPair.first ) != *lhsPair.second )
				return false;
		}

		return true;
	}
	bool operator!=( const SettingsSystem & p_lhs, const SettingsSystem & p_rhs )
	{
		if ( p_lhs._settings.size() != p_rhs._settings.size() )
			return true;

		for ( const auto & lhsPair : p_lhs._settings )
		{
			if ( !p_rhs._settings.contains( lhsPair.first ) || *p_rhs._settings.at( lhsPair.first ) != *lhsPair.second )
			{
				return true;
			}
		}

		return false;
	}
} // namespace VTX::App::Core::Settings
