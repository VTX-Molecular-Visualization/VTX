#include "ui/qt/widget/selection.hpp"
#include <QGroupBox>
#include <app/action/action_manager.hpp>
#include <app/action/system.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>
#include <util/chrono.hpp>

namespace VTX::UI::QT::Widget
{
	Selection::Selection( QWidget * const p_parent ) : QWidget( p_parent )
	{
		Util::ScopedChrono timer( "QT::Widget::Selection::Selection" );

		using namespace App;
		using namespace Core::Struct;

		_layout = new QVBoxLayout( this );
		_layout->setContentsMargins( 0, 0, 0, 0 );
		setLayout( _layout );

		_list = new QListWidget( this );
		_layout->addWidget( _list );

		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();

		Index countSystem  = 0;
		Index countChain   = 0;
		Index countResidue = 0;
		Index countAtom	   = 0;

		// Add selected items.
		// TODO: optimize.
		for ( auto entity : entities )
		{
			_entities.push_back( entity );

			const auto & topology  = reg.get<Core::Struct::Topology>( entity );
			const auto & selection = reg.get<App::System::Selection>( entity );
			const auto & metadata  = reg.get<App::System::Metadata>( entity );
			const auto & transform = reg.get<Util::Math::Transform>( entity );

			QString pdb = QString::fromStdString( metadata.pdbIDCode );

			const auto systemState = Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				countSystem++;
				countChain += topology.getChainCount();
				countResidue += topology.getResidueCount();
				countAtom += topology.getAtomCount();

				// Name.
				QString name	  = QString::fromStdString( metadata.name );
				auto *	labelName = new QLabel( QString( "Name: %1" ).arg( name ), this );
				labelName->setWordWrap( true );
				labelName->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
				_layout->addWidget( labelName );

				// PDB.
				QString pdb = QString::fromStdString( metadata.pdbIDCode );
				_layout->addWidget( new QLabel( QString( "PDB: %1" ).arg( pdb ), this ) );

				// Path.
				QString path	  = QString::fromStdString( metadata.path.string() );
				auto *	labelFile = new QLabel(
					QString( "File: %1" ).arg( QString::fromStdString( metadata.path.filename().string() ) ), this
				);
				labelFile->setWordWrap( true );
				labelFile->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
				labelFile->setToolTip( path );
				labelFile->setCursor( Qt::WhatsThisCursor );
				_layout->addWidget( labelFile );

				// Transform.
				auto * groupBoxTransform = new QGroupBox( "Transform", this );
				auto * layout			 = new QVBoxLayout( groupBoxTransform );

				_transform = new Transform( this );
				_transform->setTransform( transform );

				connect(
					_transform,
					&Transform::positionChanged,
					[ entity ]( const Vec3f & p_position )
					{ App::ACTION().execute<App::Action::System::SetPosition>( entity, p_position ); }
				);
				connect(
					_transform,
					&Transform::rotationChanged,
					[ entity ]( const Quatf & p_rotation )
					{ App::ACTION().execute<App::Action::System::SetRotation>( entity, p_rotation ); }
				);
				connect(
					_transform,
					&Transform::scaleChanged,
					[ entity ]( const Vec3f & p_scale )
					{ App::ACTION().execute<App::Action::System::SetScale>( entity, p_scale ); }
				);

				layout->addWidget( _transform );
				_layout->addWidget( groupBoxTransform );

				_list->addItem( pdb );

				continue;
			}
			else if ( systemState == App::System::E_SELECTION_STATE::NONE )
			{
				continue;
			}
			countSystem++;

			// Chains.
			for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
			{
				QString	   chainName  = QString::fromStdString( topology.getChainName( chain ) );
				const auto chainState = Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::CHAIN, chain } );
				if ( chainState == App::System::E_SELECTION_STATE::FULL )
				{
					countChain++;
					countResidue += topology.getChainResidueCount( chain );
					countAtom += topology.getChainAtomCount( chain );
					_list->addItem( pdb + "/" + chainName );
					continue;
				}
				else if ( chainState == App::System::E_SELECTION_STATE::NONE )
				{
					continue;
				}
				countChain++;

				// Residues.
				for ( Index residue : topology.getChainResidueRange( chain ) )
				{
					QString	   residueName = QString::fromStdString( topology.getResidueName( residue ) );
					const auto residueState
						= Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::RESIDUE, residue } );
					if ( residueState == App::System::E_SELECTION_STATE::FULL )
					{
						countResidue++;
						countAtom += topology.getResidueAtomCount( residue );
						_list->addItem( pdb + "/" + chainName + "/" + residueName );
						continue;
					}
					else if ( residueState == App::System::E_SELECTION_STATE::NONE )
					{
						continue;
					}
					countResidue++;

					// Atoms.
					for ( Index atom : topology.getResidueAtomRange( residue ) )
					{
						if ( Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::ATOM, atom } )
							 == App::System::E_SELECTION_STATE::FULL )
						{
							countAtom++;
							_list->addItem(
								pdb + "/" + chainName + "/" + residueName + "/"
								+ QString::fromStdString( topology.getAtomName( atom ) )
							);
						}
					}
				}
			}
		}

		// Display counters.
		if ( countSystem > 1 )
		{
			_layout->addWidget( new QLabel( QString( "Systems: %1" ).arg( countSystem ), this ) );
		}
		_layout->addWidget( new QLabel( QString( "Chains: %1" ).arg( countChain ), this ) );
		_layout->addWidget( new QLabel( QString( "Residues: %1" ).arg( countResidue ), this ) );
		_layout->addWidget( new QLabel( QString( "Atoms: %1" ).arg( countAtom ), this ) );

		// Connect.
		_connTransformChanged
			= App::REG().on_update<Util::Math::Transform>().connect<&Selection::_transformUpdated>( this );
	}

	Selection::~Selection() { _connTransformChanged.release(); }

	void Selection::_transformUpdated( App::ECS::Registry & p_reg, App::ECS::Entity p_entity )
	{
		if ( std::find( _entities.begin(), _entities.end(), p_entity ) == _entities.end() )
		{
			return;
		}

		QSignalBlocker blocker( _transform );
		const auto &   transform = p_reg.get<Util::Math::Transform>( p_entity );
		_transform->setTransform( transform );
	}
} // namespace VTX::UI::QT::Widget
