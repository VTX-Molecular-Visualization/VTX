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
#include <app/scene/viewpoint.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::Widget::Tree
{
	Camera::Camera( const Entity p_entity, QWidget * p_parent ) : BaseTree( p_parent ), _entity( p_entity )
	{
		setExpandsOnDoubleClick( true );

		addTopLevelItem( new QTreeWidgetItem( QStringList() << "Camera" ) );
		topLevelItem( 0 )->setIcon( 0, STYLE().iconFromCodepoint( Style::Icons::CAMERA ) );

		_onConstructConnection
			= App::REG().on_construct<App::Scene::ViewPoint>().connect<&Camera::_addViewPoint>( this );
		_onDestroyConnection
			= App::REG().on_destroy<App::Scene::ViewPoint>().connect<&Camera::_removeViewPoint>( this );
		App::HUB().connect<App::Events::ViewPointRenamed, &Camera::_onViewPointRenamed>( this );

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

	Camera::~Camera()
	{
		_onConstructConnection.release();
		_onDestroyConnection.release();
		App::HUB().disconnectAllOf( *this );
	}

	void Camera::_addViewPoint( Registry & p_r, const Entity p_viewpoint )
	{
		if ( _entityToItemMap.contains( p_viewpoint ) )
		{
			return;
		}

		const auto & name = p_r.get<App::Generic::Name>( p_viewpoint );

		QTreeWidgetItem * const item = new QTreeWidgetItem( QStringList() << QString::fromStdString( name.name ) );
		item->setData( 0, Qt::UserRole, QVariant::fromValue( p_viewpoint ) );

		topLevelItem( 0 )->addChild( item );
		topLevelItem( 0 )->sortChildren( 0, Qt::AscendingOrder );
		topLevelItem( 0 )->setExpanded( true );
		_entityToItemMap.emplace( p_viewpoint, item );
		updateGeometry();
	}

	void Camera::_removeViewPoint( Registry &, const Entity p_viewpoint )
	{
		const auto it = _entityToItemMap.find( p_viewpoint );
		if ( it == _entityToItemMap.end() )
		{
			return;
		}

		delete it->second;
		_entityToItemMap.erase( it );
		updateGeometry();
	}

	void Camera::_onViewPointRenamed( const App::Events::ViewPointRenamed & p_event )
	{
		const auto it = _entityToItemMap.find( p_event.viewpoint );
		if ( it == _entityToItemMap.end() )
		{
			return;
		}

		it->second->setText( 0, QString::fromStdString( p_event.name ) );
		topLevelItem( 0 )->sortChildren( 0, Qt::AscendingOrder );
	}
} // namespace VTX::UI::QT::Widget::Tree
