#ifndef __VTX_BASE_GAMEPAD_CONTROLLER__
#define __VTX_BASE_GAMEPAD_CONTROLLER__

#include "base_controller.hpp"
#include "tool/logger.hpp"
#include <QDebug>
#include <QObject>
#include <QtGamepad/QGamepad>
#include <set>

namespace VTX
{
	namespace Controller
	{
		class BaseGamepadController : public QObject, virtual public BaseController
		{
			Q_OBJECT

		  public:
			enum BUTTON
			{
				A,
				B,
				X,
				Y,
				L1,
				R1,
				START,
				SELECT,
				GUIDE
			};

			explicit BaseGamepadController( QObject * p_parent = nullptr ) : QObject( p_parent )
			{
				connect( QGamepadManager::instance(),
						 &QGamepadManager::gamepadConnected,
						 this,
						 []( int deviceID ) { VTX_DEBUG( "-----------------------GAMEPAD OK" ); } );

				_setupGamepad();

				if ( isGamepadeConnected() )
				{
					connect( _gamepad,
							 &QGamepad::axisLeftXChanged,
							 this,
							 [ & ]( double p_value )
							 {
								 VTX_DEBUG( "X: " + std::to_string( p_value ) );
								 _axisLeft.x = p_value;
							 } );
					connect( _gamepad,
							 &QGamepad::axisLeftYChanged,
							 this,
							 [ & ]( double p_value )
							 {
								 VTX_DEBUG( "Y: " + std::to_string( p_value ) );
								 _axisLeft.y = p_value;
							 } );
					connect( _gamepad,
							 &QGamepad::axisRightXChanged,
							 this,
							 [ & ]( double p_value ) { _axisRight.x = p_value; } );

					connect( _gamepad,
							 &QGamepad::axisRightYChanged,
							 this,
							 [ & ]( double p_value ) { _axisRight.y = p_value; } );

					connect( _gamepad,
							 &QGamepad::buttonAChanged,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::A, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonBChanged,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::B, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonXChanged,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::X, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonYChanged,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::Y, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonL1Changed,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::L1, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonR1Changed,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::R1, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonStartChanged,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::START, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonSelectChanged,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::SELECT, p_pressed ); } );
					connect( _gamepad,
							 &QGamepad::buttonGuideChanged,
							 this,
							 [ & ]( bool p_pressed ) { _handleButtonChangeEvent( BUTTON::GUIDE, p_pressed ); } );

					connect( _gamepad,
							 &QGamepad::buttonL2Changed,
							 this,
							 [ & ]( double p_value ) { _triggers.x = p_value; } );
					connect( _gamepad,
							 &QGamepad::buttonR2Changed,
							 this,
							 [ & ]( double p_value ) { _triggers.y = p_value; } );
				}
			}
			~BaseGamepadController()
			{
				if ( _gamepad != nullptr )
				{
					delete _gamepad;
				}
			}

			inline const bool isGamepadeConnected() const { return _gamepad != nullptr; }

		  protected:
			const QGamepad * _gamepad		 = nullptr;
			std::set<int>	 _pressedButtons = std::set<int>();
			Vec2f			 _axisLeft		 = Vec2f();
			Vec2f			 _axisRight		 = Vec2f();
			Vec2f			 _triggers		 = Vec2f();

			void _setupGamepad()
			{
				if ( _gamepad != nullptr )
				{
					delete _gamepad;
				}

				const QList<int> gamepads = QGamepadManager::instance()->connectedGamepads();
				if ( gamepads.isEmpty() == false )
				{
					_gamepad = new QGamepad( *gamepads.begin(), this );
					VTX_DEBUG( "Gamepad connected" );
				}
				else
				{
					VTX_DEBUG( "Gamepad not connected" );
				}
			}

			virtual void _handleButtonChangeEvent( const BUTTON & p_button, const bool p_pressed )
			{
				VTX_DEBUG( "button change " + std::to_string( p_button ) );
				if ( p_pressed )
				{
					_handleButtonDownEvent( p_button );
				}
				else
				{
					_handleButtonUpEvent( p_button );
				}
			}

			virtual void _handleButtonDownEvent( const BUTTON & p_button ) { _pressedButtons.emplace( p_button ); }
			virtual void _handleButtonUpEvent( const BUTTON & p_button ) { _pressedButtons.erase( p_button ); }
		};
	} // namespace Controller
} // namespace VTX
#endif
