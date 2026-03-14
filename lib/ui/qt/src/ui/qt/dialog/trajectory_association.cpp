#include "ui/qt/dialog/trajectory_association.hpp"
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVariant>
#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/action/scene.hpp>
#include <app/ecs.hpp>
#include <app/events.hpp>
#include <app/services.hpp>
#include <app/system/metadata.hpp>
#include <app/system/trajectory.hpp>
#include <core/struct/system.hpp>
#include <qboxlayout.h>
#include <util/event_hub.hpp>

Q_DECLARE_METATYPE( VTX::App::ECS::Entity ) // used to allow QVariant conversion

namespace VTX::UI::QT::Dialog
{
	TrajectoryAssociation::TrajectoryAssociation( FilePath p_path ) : _path( std::move( p_path ) )
	{
		setWindowTitle( "Assign structure" );
		setFixedSize( 300, 180 );

		auto * layoutCacheRadio = new QVBoxLayout( this );

		_cbSystem = new QComboBox( this );
		_cbSystem->setEditable( true );
		_cbSystem->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		for ( auto & it_entity : App::REG().view<App::System::Metadata>() )
		{
			auto &	metadata	  = App::REG().get<App::System::Metadata>( it_entity );
			QString displayString = QString::fromStdString( metadata.name );
			_cbSystem->addItem( displayString, QVariant::fromValue( it_entity ) );
		}
		_cbSystem->addItem( "New structure", QVariant::fromValue( VTX::App::ECS::Entity( entt::null ) ) );

		_cbSystem->setCurrentIndex( 0 );
		layoutCacheRadio->addWidget( _cbSystem.data() );
		QDialogButtonBox * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open, this
		);
		layoutCacheRadio->addWidget( buttonBox );

		connect( buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
		connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );

		connect(
			this,
			&QDialog::accepted,
			[ this ]()
			{
				App::ECS::Entity entity = _cbSystem->currentData().value<App::ECS::Entity>();
				if ( entity == entt::null )

					App::ACTION().execute<App::Action::IO::LoadSystem>( _path );
				else
					App::ACTION().execute<App::Action::IO::AssociateTrajectory>( _path, entity );
			}
		);
	}

} // namespace VTX::UI::QT::Dialog
