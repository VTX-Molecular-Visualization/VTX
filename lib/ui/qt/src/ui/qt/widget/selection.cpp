#include "ui/qt/widget/selection.hpp"
#include "ui/qt/helper.hpp"
#include <QDesktopServices>
#include <QGroupBox>
#include <QLabel>
#include <QMenu>
#include <QPoint>
#include <QUrl>
#include <app/action/action_manager.hpp>
#include <app/action/system.hpp>
#include <app/helper/system.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <io/reader.hpp>
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

		auto &	   reg					= REG();
		const auto entities				= reg.view<App::System::Selection>();
		Entity	   editableSystemEntity = InvalidEntity;
		Index	   selectedSystemCount	= 0;

		for ( const Entity entity : entities )
		{
			const auto & selection = reg.get<App::System::Selection>( entity );
			if ( selection.atoms.none() )
			{
				continue;
			}

			selectedSystemCount++;
			if ( App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } )
				 == App::System::E_SELECTION_STATE::FULL )
			{
				editableSystemEntity = entity;
			}
		}

		const bool showEditableSystemFields = selectedSystemCount == 1 && editableSystemEntity != InvalidEntity;

		if ( showEditableSystemFields )
		{
			const auto & metadata  = reg.get<IO::Metadata>( editableSystemEntity );
			const auto & transform = reg.get<Util::Math::Transform>( editableSystemEntity );

			// Name.
			_textName = new QPlainTextEdit( QString::fromStdString( metadata.name ), this );
			_textName->setPlaceholderText( "No name" );
			_textName->setLineWrapMode( QPlainTextEdit::WidgetWidth );
			_textName->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Fixed );
			_textName->setFixedHeight( _textName->fontMetrics().lineSpacing() * 4 );
			_layout->addWidget( _textName );

			connect(
				_textName,
				&QPlainTextEdit::textChanged,
				this,
				[ this, editableSystemEntity ]()
				{
					App::ACTION().execute<App::Action::System::SetName>(
						editableSystemEntity, _textName->toPlainText().toStdString()
					);
				}
			);

			// PDB.
			_layout->addWidget(
				new QLabel( QString( "PDB: %1" ).arg( QString::fromStdString( metadata.pdbIDCode ) ), this )
			);

			// Path.
			const QUrl parentFolderUrl
				= QUrl::fromLocalFile( QString::fromStdString( metadata.path.parent_path().string() ) );
			auto * labelFile = new QLabel(
				QString( "File: %1" ).arg( QString::fromStdString( metadata.path.filename().string() ) ), this
			);
			labelFile->setWordWrap( true );
			labelFile->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
			labelFile->setToolTip( QString::fromStdString( metadata.path.generic_string() ) );
			labelFile->setCursor( Qt::WhatsThisCursor );
			labelFile->setContextMenuPolicy( Qt::CustomContextMenu );
			connect(
				labelFile,
				&QLabel::customContextMenuRequested,
				this,
				[ labelFile, parentFolderUrl ]( const QPoint & p_pos )
				{
					QMenu menu( labelFile );
					menu.addAction(
						"Show in explorer",
						labelFile,
						[ parentFolderUrl ]() { QDesktopServices::openUrl( parentFolderUrl ); }
					);
					menu.exec( labelFile->mapToGlobal( p_pos ) );
				}
			);
			_layout->addWidget( labelFile );

			// Transform.
			auto * groupBoxTransform = new QGroupBox( "Transform", this );
			auto * layout			 = new QVBoxLayout( groupBoxTransform );

			_transform = new Transform( this, Transform::E_FLAG::POSITION | Transform::E_FLAG::ROTATION );
			_transform->setTransform( transform );

			connect(
				_transform,
				&Transform::positionChanged,
				[ editableSystemEntity ]( const Vec3f & p_position )
				{ App::ACTION().execute<App::Action::System::SetPosition>( editableSystemEntity, p_position ); }
			);
			connect(
				_transform,
				&Transform::rotationChanged,
				[ editableSystemEntity ]( const Quatf & p_rotation )
				{ App::ACTION().execute<App::Action::System::SetRotation>( editableSystemEntity, p_rotation ); }
			);
			connect(
				_transform,
				&Transform::scaleChanged,
				[ editableSystemEntity ]( const Vec3f & p_scale )
				{ App::ACTION().execute<App::Action::System::SetScale>( editableSystemEntity, p_scale ); }
			);

			layout->addWidget( _transform );
			_layout->addWidget( groupBoxTransform );
		}

		Index countSystem  = 0;
		Index countChain   = 0;
		Index countResidue = 0;
		Index countAtom	   = 0;

		// Add selected items.
		// TODO: optimize.
		for ( auto entity : entities )
		{
			const auto & topology  = reg.get<Core::Struct::Topology>( entity );
			const auto & selection = reg.get<App::System::Selection>( entity );
			if ( selection.atoms.none() )
			{
				continue;
			}

			_entities.push_back( entity );

			const auto & metadata = reg.get<IO::Metadata>( entity );

			QString systemName = ( metadata.pdbIDCode == IO::PDB_ID_CODE_DEFAULT )
									 ? QString::fromStdString( metadata.path.stem().string() )
									 : QString::fromStdString( metadata.pdbIDCode );

			const auto systemState = App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::SYSTEM } );
			if ( systemState == App::System::E_SELECTION_STATE::FULL )
			{
				countSystem++;
				countChain += topology.getChainCount();
				countResidue += topology.getResidueCount();
				countAtom += topology.getAtomCount();

				_list->addItem( systemName );

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
				QString	   chainName = QString::fromStdString( topology.getChainName( chain ) );
				const auto chainState
					= App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::CHAIN, chain } );
				if ( chainState == App::System::E_SELECTION_STATE::FULL )
				{
					countChain++;
					countResidue += topology.getChainResidueCount( chain );
					countAtom += topology.getChainAtomCount( chain );
					_list->addItem( systemName + "/" + chainName );
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
						= App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::RESIDUE, residue } );
					if ( residueState == App::System::E_SELECTION_STATE::FULL )
					{
						countResidue++;
						countAtom += topology.getResidueAtomCount( residue );
						_list->addItem( systemName + "/" + chainName + "/" + residueName );
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
						if ( App::Helper::System::getSelectionState( { entity, E_SYSTEM_ITEM::ATOM, atom } )
							 == App::System::E_SELECTION_STATE::FULL )
						{
							countAtom++;
							_list->addItem(
								systemName + "/" + chainName + "/" + residueName + "/"
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
			_layout->addWidget(
				new QLabel( QString( "Systems: %1" ).arg( VTX::UI::QT::Helper::formatNumber( countSystem ) ), this )
			);
		}
		_layout->addWidget(
			new QLabel( QString( "Chains: %1" ).arg( VTX::UI::QT::Helper::formatNumber( countChain ) ), this )
		);
		_layout->addWidget(
			new QLabel( QString( "Residues: %1" ).arg( VTX::UI::QT::Helper::formatNumber( countResidue ) ), this )
		);
		_layout->addWidget(
			new QLabel( QString( "Atoms: %1" ).arg( VTX::UI::QT::Helper::formatNumber( countAtom ) ), this )
		);

		// Connect.
		_connTransformChanged
			= App::REG().on_update<Util::Math::Transform>().connect<&Selection::_transformUpdated>( this );
		_connMetadataChanged = App::REG().on_update<IO::Metadata>().connect<&Selection::_metadataUpdated>( this );
	}

	Selection::~Selection()
	{
		_connTransformChanged.release();
		_connMetadataChanged.release();
	}

	void Selection::_transformUpdated( Registry & p_reg, Entity p_entity )
	{
		if ( !_transform || std::find( _entities.begin(), _entities.end(), p_entity ) == _entities.end() )
		{
			return;
		}

		QSignalBlocker blocker( _transform );
		const auto &   transform = p_reg.get<Util::Math::Transform>( p_entity );
		_transform->setTransform( transform );
	}

	void Selection::_metadataUpdated( Registry & p_reg, Entity p_entity )
	{
		if ( !_textName || std::find( _entities.begin(), _entities.end(), p_entity ) == _entities.end() )
		{
			return;
		}

		const auto &  metadata = p_reg.get<IO::Metadata>( p_entity );
		const QString name	   = QString::fromStdString( metadata.name );
		if ( _textName->toPlainText() == name )
		{
			return;
		}

		QSignalBlocker blocker( _textName );
		_textName->setPlainText( name );
	}
} // namespace VTX::UI::QT::Widget
