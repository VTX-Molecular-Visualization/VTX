#ifndef __VTX_UI_QT_CONTROLLER_SHORTCUT_MAP__
#define __VTX_UI_QT_CONTROLLER_SHORTCUT_MAP__

#include "ui/qt/controller/shortcut.hpp"
#include "ui/qt/input/input_manager.hpp"
#include <functional>
#include <map>
#include <set>

namespace VTX::UI::QT::Controller
{
	class ShortcutMap
	{
	  public:
		ShortcutMap()								= default;
		ShortcutMap( const ShortcutMap & p_source ) = default;

		void referenceShortcut( const Shortcut & p_shortcut );
		void removeShortcut( const Shortcut & p_shortcut );

		const Shortcut * tryGet( const Input::Key & p_key, const Input::ModifierFlag & p_modifiers ) const;

	  private:
		std::map<Input::Key, std::set<Shortcut>> _mapTriggerKeyShortcut = std::map<Input::Key, std::set<Shortcut>>();
	};
} // namespace VTX::UI::QT::Controller
#endif
