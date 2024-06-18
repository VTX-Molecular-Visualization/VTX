#include "app/core/input/shortcut_map.hpp"

namespace VTX::App::Core::Input
{
	void ShortcutMap::referenceShortcut( const Shortcut & p_shortcut )
	{
		_mapTriggerKeyShortcut[ p_shortcut.getSequence().getKey() ].emplace( p_shortcut );
	}

	const Shortcut * ShortcutMap::tryGet( const Input::Key & p_key, const Input::ModifierFlag & p_modifiers ) const
	{
		auto it = _mapTriggerKeyShortcut.find( p_key );

		if ( it == _mapTriggerKeyShortcut.end() )
			return nullptr;

		for ( const Shortcut & shortcut : it->second )
		{
			if ( shortcut.getSequence().match( p_key, p_modifiers ) )
			{
				return &shortcut;
			}
		}

		return nullptr;
	}
} // namespace VTX::App::Core::Input
