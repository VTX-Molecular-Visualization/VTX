#include "app/core/settings/settings_system.hpp"

namespace VTX::App::Core::Settings
{
	SettingsSystem::SettingsSystem( const SettingsSystem & p_source )
	{
		for ( const auto & pair : p_source._representation )
		{
			_representation.emplace( pair.first, pair.second->clone() );
		}
	}

	void SettingsSystem::reset()
	{
		for ( auto & pair : _representation )
		{
			pair.second->reset();
		}
	}

	bool operator==( const SettingsSystem & p_lhs, const SettingsSystem & p_rhs )
	{
		if ( p_lhs._representation.size() != p_rhs._representation.size() )
			return false;

		for ( const auto & lhsPair : p_lhs._representation )
		{
			if ( !p_rhs._representation.contains( lhsPair.first ) || *p_rhs._representation.at( lhsPair.first ) != *lhsPair.second )
				return false;
		}

		return true;
	}
	bool operator!=( const SettingsSystem & p_lhs, const SettingsSystem & p_rhs )
	{
		if ( p_lhs._representation.size() != p_rhs._representation.size() )
			return true;

		for ( const auto & lhsPair : p_lhs._representation )
		{
			if ( !p_rhs._representation.contains( lhsPair.first ) || *p_rhs._representation.at( lhsPair.first ) != *lhsPair.second )
			{
				return true;
			}
		}

		return false;
	}
} // namespace VTX::App::Core::Settings
