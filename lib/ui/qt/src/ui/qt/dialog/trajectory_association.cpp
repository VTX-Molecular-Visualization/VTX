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
#include <app/system/trajectory.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <qboxlayout.h>
#include <util/event_hub.hpp>

Q_DECLARE_METATYPE( VTX::Entity ) // used to allow QVariant conversion

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

		for ( auto & it_entity : App::REG().view<IO::Metadata>() )
		{
			auto & metadata = App::REG().get<IO::Metadata>( it_entity );

			QString systemName = ( metadata.pdbIDCode == IO::PDB_ID_CODE_DEFAULT )
									 ? QString::fromStdString( metadata.path.stem().string() )
									 : QString::fromStdString( metadata.pdbIDCode );

			_cbSystem->addItem( systemName, QVariant::fromValue( it_entity ) );
		}
		_cbSystem->addItem( "New structure", QVariant::fromValue( VTX::Entity( entt::null ) ) );

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
				Entity entity = _cbSystem->currentData().value<Entity>();
				if ( entity == entt::null )
				{
					App::ACTION().execute<App::Action::IO::LoadSystem>( _path );
				}
				else
				{
					App::ACTION().execute<App::Action::IO::AssociateTrajectory>( _path, entity );
				}
			}
		);
	}

} // namespace VTX::UI::QT::Dialog
