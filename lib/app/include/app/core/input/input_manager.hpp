#ifndef __VTX_UI_QT_INPUT_INPUT_MANAGER__
#define __VTX_UI_QT_INPUT_INPUT_MANAGER__

#include "events.hpp"
#include <app/application/system/system_registration.hpp>
#include <map>
#include <queue>
#include <set>
#include <util/callback.hpp>
#include <util/concepts.hpp>
#include <util/types.hpp>

namespace VTX::App::Core::Input
{
	class InputManager : public App::Application::System::AutoRegistrateSystem<InputManager>
	{
	  private:
		inline static const int CLICK_MAX_DISTANCE = 3;

	  public:
		static KeyboardLayout getKeyboardLayout();
		static Key			  getKeyFromQwerty( const Key p_key );

	  private:
		static ModifierEnum _getModifierFromKey( const Key & p_key );

	  public:
		InputManager();
		InputManager( const InputManager & )			 = delete;
		InputManager & operator=( const InputManager & ) = delete;
		~InputManager();

		// Keyboard
		void handleKeyboardEvent( const KeyEvent & p_event );
		bool isKeyPressed( const Key & p_key ) const;

		template<ContainerOfType<Key> C>
		bool isAnyKeyPressed( const C & p_keys ) const
		{
			for ( const Key & key : p_keys )
				if ( isKeyPressed( key ) )
					return true;

			return false;
		}

		inline const ModifierFlag & getCurrentModifiers() const { return _modifiers; }
		bool						isModifier( const ModifierFlag & p_modifier ) const;
		bool						isModifierExclusive( const ModifierFlag & p_modifier ) const;
		void						clearKeyboardBuffer();

		Util::Callback<Key> onKeyPressed;
		Util::Callback<Key> onKeyReleased;

		// Mouse
		void		  handleMouseEvent( const MouseEvent & p_event );
		void		  handleMouseWheelEvent( const WheelEvent & p_event );
		bool		  isMouseLeftPressed() const;
		bool		  isMouseRightPressed() const;
		bool		  isMouseMiddlePressed() const;
		const Vec2i & getCurrentMousePosition() const;
		const Vec2i & getDeltaMousePosition() const;
		const Vec2i & getMouseLeftClickPosition() const;
		const Vec2i & getMouseRightClickPosition() const;
		int			  getDeltaMouseWheel() const;

		Util::Callback<Vec2i> onMouseLeftClicked;
		Util::Callback<Vec2i> onMouseRightClicked;
		Util::Callback<Vec2i> onMouseLeftDoubleClicked;

		// void registerEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const );
		// void unregisterEventReceiverKeyboard( VTX::Event::BaseEventReceiverKeyboard * const );
		// void registerEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const );
		// void unregisterEventReceiverMouse( VTX::Event::BaseEventReceiverMouse * const );
		// void registerEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const );
		// void unregisterEventReceiverWheel( VTX::Event::BaseEventReceiverWheel * const );

		// void clearKeyboardInputEvents() const;
		// void clearKeyboardInputEvent( const Qt::Key & p_key ) const;

		void freezeEvent( const bool p_freeze );

	  private:
		// Mouse
		void _handleMouseButtonDownEvent( const MouseEvent & p_event );
		void _handleMouseButtonUpEvent( const MouseEvent & p_event );
		void _handleMouseDoubleClickEvent( const MouseEvent & p_event );
		void _handleMouseMotionEvent( const MouseEvent & p_event );
		void _handleMouseWheelEvent( const WheelEvent & p_event );

		// void fireEventKeyboard( QKeyEvent * const, const App::Old::VTX_ID & );
		// void fireEventMouse( QMouseEvent * const, const App::Old::VTX_ID & );
		// void fireEventWheel( QWheelEvent * const, const App::Old::VTX_ID & );

	  private:
		// Input events.
		// std::set<VTX::Event::BaseEventReceiverKeyboard *> _receiversKeyboard
		//	= std::set<VTX::Event::BaseEventReceiverKeyboard *>();
		// std::set<VTX::Event::BaseEventReceiverMouse *> _receiversMouse
		//	= std::set<VTX::Event::BaseEventReceiverMouse *>();
		// std::set<VTX::Event::BaseEventReceiverWheel *> _receiversWheel
		//	= std::set<VTX::Event::BaseEventReceiverWheel *>();

		// Keyboard
		std::set<Key> _pressedKeys = std::set<Key>();
		ModifierFlag  _modifiers   = ModifierEnum::None;

		// Mouse
		bool  _mouseLeftPressed		   = false;
		Vec2i _mouseLeftClickPosition  = Vec2i();
		bool  _mouseRightPressed	   = false;
		Vec2i _mouseRightClickPosition = Vec2i();
		bool  _mouseMiddlePressed	   = false;

		Vec2i _mousePosition	  = Vec2i();
		Vec2i _deltaMousePosition = Vec2i();
		int	  _deltaMouseWheel	  = 0;

		bool _isLeftClickCanceled  = false;
		bool _isRightClickCanceled = false;

		bool _freeze = false;

		// void _flushEventKeyboard( QKeyEvent * const, const App::Old::VTX_ID & );
		// void _flushEventMouse( QMouseEvent * const, const App::Old::VTX_ID & );
		// void _flushEventWheel( QWheelEvent * const, const App::Old::VTX_ID & );
	};
} // namespace VTX::App::Core::Input

namespace VTX::App
{
	Core::Input::InputManager & INPUT_MANAGER();
}

#endif
