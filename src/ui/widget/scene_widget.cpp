#include "vtx_app.hpp"
#include "object3d/scene.hpp"
#include "model/molecule.hpp"
#include "scene_widget.hpp"
#include <QListWidget>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			SceneWidget::SceneWidget( QWidget * p_parent ) : QDockWidget( p_parent ), _ui( new Ui_SceneWidget() )
			{
				Event::BaseEventReceiverVTX::_registerEvents();
				_ui->setupUi( this );
			}

			SceneWidget::~SceneWidget()
			{
				Event::BaseEventReceiverVTX::_unregisterEvents();
				delete ( _ui );
			}

			void SceneWidget::receiveEvent( const Event::VTXEvent & p_event )
			{
				if ( p_event.name == Event::Global::ON_SCENE_CHANGE )
				{
					// TODO
				}
			}

		} // namespace Widget
	}// namespace UI
} // namespace VTX
