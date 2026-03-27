#ifndef __VTX_UI_QT_MENU_CAMERA__
#define __VTX_UI_QT_MENU_CAMERA__

#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>
#include <app/action/controller.hpp>
#include <app/events.hpp>
#include <app/setting/controller.hpp>
#include <renderer/camera.hpp>
#include <util/event_hub.hpp>
#include <util/type_traits.hpp>

namespace VTX::UI::QT::Menu
{
	class Camera : public Widget::BaseWidget<Camera, QMenu>
	{
	  public:
		Camera( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setTitle( "Camera" );
			_aPerspective = addAction<Action::Camera::Perspective>();
			_aOrtho		  = addAction<Action::Camera::Orthographic>();
			addSeparator();
			_aTrackball = addAction<Action::Camera::Trackball>();
			_aFreefly	= addAction<Action::Camera::Freefly>();
			addSeparator();
			addAction<Action::Camera::Orient>();
			addAction<Action::Camera::Reset>();

			// Connect.
			App::REG().on_update<Renderer::Camera>().connect<&Camera::_onProjectionChanged>( this );
			App::REG().on_update<App::Setting::Controller>().connect<&Camera::_onControllerChanged>( this );
		}

	  private:
		QPointer<QAction> _aPerspective;
		QPointer<QAction> _aOrtho;
		QPointer<QAction> _aTrackball;
		QPointer<QAction> _aFreefly;

		void _onProjectionChanged( App::ECS::Registry &, const App::ECS::Entity p_e )
		{
			const auto & camera = App::REG().get<Renderer::Camera>( p_e );

			if ( camera.projection == Renderer::PROJECTION::PERSPECTIVE )
			{
				_aPerspective->setChecked( true );
			}
			else if ( camera.projection == Renderer::PROJECTION::ORTHOGRAPHIC )
			{
				_aOrtho->setChecked( true );
			}
		}

		void _onControllerChanged( App::ECS::Registry &, const App::ECS::Entity p_e )
		{
			using namespace App::Setting;

			const auto & setting = App::REG().get<Controller>( p_e );

			E_CONTROLLER type = static_cast<E_CONTROLLER>( setting.current );

			switch ( type )
			{
			case E_CONTROLLER::FREEFLY: _aFreefly->setChecked( true ); break;
			case E_CONTROLLER::TRACKBALL: _aTrackball->setChecked( true ); break;
			default: break;
			}
		}
	};

} // namespace VTX::UI::QT::Menu

#endif
