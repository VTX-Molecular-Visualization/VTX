#ifndef __VTX_UI_QT_CONTROLLER_BASE_SHORTCUT_CONTROLLER__
#define __VTX_UI_QT_CONTROLLER_BASE_SHORTCUT_CONTROLLER__

#include "ui/core/input/keys.hpp"
#include "ui/core/input/shortcut.hpp"
#include "ui/core/input/shortcut_map.hpp"
#include "ui/qt/controller/base_controller.hpp"

namespace VTX::UI::QT::Controller
{
	class BaseShortcutController : public BaseController
	{
	  public:
		BaseShortcutController()										  = default;
		BaseShortcutController( const BaseShortcutController & p_source ) = default;
		~BaseShortcutController()										  = default;

		virtual void init() override;
		void		 referenceShortcut( const Core::Input::Shortcut & p_shortcut );

	  protected:
		void _onKeyPressed( Core::Input::Key p_key ) const;

		Core::Input::ShortcutMap _mapShortcuts = Core::Input::ShortcutMap();
	};
} // namespace VTX::UI::QT::Controller
#endif
