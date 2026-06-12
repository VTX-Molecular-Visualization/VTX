#include "ui/qt/widget/tree/camera.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QStringList>
#include <QTreeWidgetItem>
#include <QVariant>
#include <app/action/action_manager.hpp>
#include <app/action/camera.hpp>
#include <app/events.hpp>
#include <app/generic/name.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget::Tree
{
	Camera::Camera( const Entity p_entity, QWidget * p_parent ) : BaseTree( p_parent ), _entity( p_entity )
	{
		setExpandsOnDoubleClick( true );

		addTopLevelItem( new QTreeWidgetItem( QStringList() << "Camera" ) );
		topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::CAMERA ) );

		App::HUB().connect<App::Events::ViewPointAdded, &Camera::_onViewPointAdded>( this );
		App::HUB().connect<App::Events::ViewPointDeleted, &Camera::_onViewPointDeleted>( this );

		// Connect double click to move to viewpoint.
		connect(
			this,
			&QTreeWidget::itemDoubleClicked,
			this,
			[ this ]( QTreeWidgetItem * item, int column )
			{
				if ( item == nullptr || item->parent() == nullptr || column != 0 )
				{
					return;
				}

				Entity entity = item->data( 0, Qt::UserRole ).value<Entity>();
				App::ACTION().execute<App::Action::Camera::GoToViewPoint>( entity );
			}
		);
	}

	Camera::~Camera() { App::HUB().disconnectAllOf( *this ); }

	void Camera::_onViewPointAdded( const App::Events::ViewPointAdded & p_event )
	{
		if ( _entityToItemMap.contains( p_event.viewpoint ) )
		{
			return;
		}

		const auto & name = App::REG().get<App::Generic::Name>( p_event.viewpoint );

		QTreeWidgetItem * const item = new QTreeWidgetItem( QStringList() << QString::fromStdString( name.name ) );
		item->setData( 0, Qt::UserRole, QVariant::fromValue( p_event.viewpoint ) );

		topLevelItem( 0 )->addChild( item );
		topLevelItem( 0 )->sortChildren( 0, Qt::AscendingOrder );
		topLevelItem( 0 )->setExpanded( true );
		_entityToItemMap.emplace( p_event.viewpoint, item );
		updateGeometry();
	}

	void Camera::_onViewPointDeleted( const App::Events::ViewPointDeleted & p_event )
	{
		const auto it = _entityToItemMap.find( p_event.viewpoint );
		if ( it == _entityToItemMap.end() )
		{
			return;
		}

		delete it->second;
		_entityToItemMap.erase( it );
		updateGeometry();
	}
} // namespace VTX::UI::QT::Widget::Tree
