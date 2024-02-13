#ifndef __VTX_UI_CORE_INPUT_SHORTCUT_MAP__
#define __VTX_UI_CORE_INPUT_SHORTCUT_MAP__

#include "ui/core/input/keys.hpp"
#include "ui/core/input/shortcut.hpp"
#include <functional>
#include <map>
#include <set>

namespace VTX::UI::Core::Input
{
	class ShortcutMap
	{
	  public:
		ShortcutMap()								= default;
		ShortcutMap( const ShortcutMap & p_source ) = default;

		void referenceShortcut( const Shortcut & p_shortcut );
		void removeShortcut( const Shortcut & p_shortcut );

		const Shortcut * tryGet( const Key & p_key, const ModifierFlag & p_modifiers ) const;

	  private:
		std::map<Key, std::set<Shortcut>> _mapTriggerKeyShortcut = std::map<Key, std::set<Shortcut>>();
	};
} // namespace VTX::UI::Core::Input
#endif
