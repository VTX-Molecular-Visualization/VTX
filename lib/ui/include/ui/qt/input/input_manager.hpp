#ifndef __VTX_UI_QT_INPUT_INPUT_MANAGER__
#define __VTX_UI_QT_INPUT_INPUT_MANAGER__

#include "ui/core/input/keys.hpp"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <Qt>
#include <app/application/system/system_registration.hpp>
#include <app/core/callback_event.hpp>
#include <app/core/system/base_system.hpp>
#include <map>
#include <queue>
#include <set>
#include <util/concepts.hpp>
#include <util/types.hpp>

namespace VTX::UI::QT::Input
{
	using namespace UI::Core::Input;

	class InputManager : public App::Core::System::BaseSystem
	{
	  public:
		inline static App::Application::System::SystemRegistration<InputManager> SYSTEM
			= App::Application::System::SystemRegistration<InputManager>();

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
		void handleKeyboardEvent( const QKeyEvent & p_event );
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

		App::Core::CallbackEmitter<Key> onKeyPressed  = App::Core::CallbackEmitter<Key>();
		App::Core::CallbackEmitter<Key> onKeyReleased = App::Core::CallbackEmitter<Key>();

		// Mouse
		void		  handleMouseEvent( const QMouseEvent & p_event );
		void		  handleMouseWheelEvent( const QWheelEvent & p_event );
		bool		  isMouseLeftPressed() const;
		bool		  isMouseRightPressed() const;
		bool		  isMouseMiddlePressed() const;
		const Vec2i & getCurrentMousePosition() const;
		const Vec2i & getDeltaMousePosition() const;
		const Vec2i & getMouseLeftClickPosition() const;
		const Vec2i & getMouseRightClickPosition() const;
		int			  getDeltaMouseWheel() const;

		App::Core::CallbackEmitter<Vec2i> onMouseLeftClicked	   = App::Core::CallbackEmitter<Vec2i>();
		App::Core::CallbackEmitter<Vec2i> onMouseRightClicked	   = App::Core::CallbackEmitter<Vec2i>();
		App::Core::CallbackEmitter<Vec2i> onMouseLeftDoubleClicked = App::Core::CallbackEmitter<Vec2i>();

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
		void _handleMouseButtonDownEvent( const QMouseEvent & p_event );
		void _handleMouseButtonUpEvent( const QMouseEvent & p_event );
		void _handleMouseDoubleClickEvent( const QMouseEvent & p_event );
		void _handleMouseMotionEvent( const QMouseEvent & p_event );
		void _handleMouseWheelEvent( const QWheelEvent & p_event );

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
} // namespace VTX::UI::QT::Input

namespace VTX::UI::QT
{
	Input::InputManager & INPUT_MANAGER();
}

#endif