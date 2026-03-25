#include "ui/qt/widget/selection.hpp"
#include "ui/qt/services.hpp"
#include <QGroupBox>
#include <QListWidget>
#include <app/action/action_manager.hpp>
#include <app/action/system.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/metadata.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>
#include <util/chrono.hpp>
#include <util/event_hub.hpp>

namespace
{
	// TODO: use and make configurable?
	constexpr uint MAX_DISPLAYED = 50;
} // namespace

namespace VTX::UI::QT::Widget
{
	Selection::Selection( QWidget * const p_parent ) : QWidget( p_parent )
	{
		_layout = new QVBoxLayout( this );
		_layout->setContentsMargins( 0, 0, 0, 0 );
		setLayout( _layout );
		refresh();
	}

	Selection::~Selection()
	{
		App::REG().on_update<Util::Math::Transform>().disconnect<&Selection::_transformUpdated>( this );
	}

	// TODO: optimize and factorize.
	void Selection::refresh()
	{
		Util::ScopedChrono timer( "QT::Widget::Selection::_onUpdateSelection" );

		using namespace App;
		using namespace Core::Struct;

		auto &	   reg		= REG();
		const auto entities = reg.view<App::System::Selection>();

		// Temporary fix: rebuild the content from a clean layout.
		_entities.clear();
		while ( _layout->count() > 0 )
		{
			QLayoutItem * item = _layout->takeAt( 0 );
			if ( item->widget() != nullptr )
			{
				delete item->widget();
			}
			delete item;
		}
		delete _transform;

		// Add selected items.
		for ( auto entity : entities )
		{
			_entities.push_back( entity );

			const auto & topology  = reg.get<Core::Struct::Topology>( entity );
			const auto & selection = reg.get<App::System::Selection>( entity );
			const auto & metadata  = reg.get<App::System::Metadata>( entity );
			const auto & transform = reg.get<Util::Math::Transform>( entity );

			QString name = QString::fromStdString( topology.name );

			_layout->addWidget( new QLabel( QString( "Selection: %1" ).arg( selection.atoms.count() ), this ) );

			const auto systemState = Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				///////////////////////// Transform.
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

				//////////////////////// COUNT.
				_layout->addWidget( new QLabel( name, this ) );
				_layout->addWidget( new QLabel( QString( "Atoms: %1" ).arg( topology.getAtomCount() ), this ) );
				_layout->addWidget( new QLabel( QString( "Residues: %1" ).arg( topology.getResidueCount() ), this ) );
				_layout->addWidget( new QLabel( QString( "Chains: %1" ).arg( topology.getChainCount() ), this ) );
				_layout->addWidget( new QLabel( QString( "Bonds: %1" ).arg( topology.getBondCount() ), this ) );

				// addItem( name );
				continue;
			}
			else if ( systemState == App::System::E_SELECTION_STATE::NONE )
			{
				continue;
			}

			// Chains.
			for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
			{
				QString	   chainName  = QString::fromStdString( topology.getChainName( chain ) );
				const auto chainState = Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::CHAIN, chain } );
				if ( chainState == App::System::E_SELECTION_STATE::FULL )
				{
					// addItem( name + "/" + chainName );
					continue;
				}
				else if ( chainState == App::System::E_SELECTION_STATE::NONE )
				{
					continue;
				}

				// Residues.
				for ( Index residue : topology.getChainResidueRange( chain ) )
				{
					QString	   residueName = QString::fromStdString( topology.getResidueName( residue ) );
					const auto residueState
						= Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::RESIDUE, residue } );
					if ( residueState == App::System::E_SELECTION_STATE::FULL )
					{
						// addItem( name + "/" + chainName + "/" + residueName );
						continue;
					}
					else if ( residueState == App::System::E_SELECTION_STATE::NONE )
					{
						continue;
					}

					// Atoms.
					for ( Index atom : topology.getResidueAtomRange( residue ) )
					{
						if ( Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::ATOM, atom } )
							 == App::System::E_SELECTION_STATE::FULL )
						{
							// addItem(
							//	name + "/" + chainName + "/" + residueName + "/"
							//	+ QString::fromStdString( topology.getAtomName( atom ) )
							//);
						}
					}
				}
			}
		}

		App::REG().on_update<Util::Math::Transform>().connect<&Selection::_transformUpdated>( this );
	}

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
