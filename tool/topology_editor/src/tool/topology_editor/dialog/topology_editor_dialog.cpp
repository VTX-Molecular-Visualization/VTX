#include "tool/topology_editor/dialog/topology_editor_dialog.hpp"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QToolBar>
#include <QTreeView>
#include <app/services.hpp>
#include <app/system/metadata.hpp>
#include <app/system/trajectory.hpp>
#include <core/chemdb/atom.hpp>
#include <core/chemdb/bond.hpp>
#include <core/chemdb/residue.hpp>
#include <core/struct/topology.hpp>
#include <ui/qt/model/system_model.hpp>
#include <ui/qt/services.hpp>
#include <ui/qt/style/icons.hpp>
#include <ui/qt/style/style_manager.hpp>
#include <util/constants.hpp>
#include <util/enum.hpp>

namespace VTX::Tool::TopologyEditor::Dialog
{
	namespace
	{
		constexpr int MIN_DIALOG_WIDTH	= 1280;
		constexpr int MIN_DIALOG_HEIGHT = 760;

		QString _textOrDash( const std::string & p_value )
		{
			if ( p_value.empty() )
			{
				return "-";
			}
			return QString::fromStdString( p_value );
		}

		QString _entityLabel( const App::ECS::Entity p_entity )
		{
			const auto & metadata = App::REG().get<App::System::Metadata>( p_entity );
			if ( not metadata.name.empty() )
			{
				return QString::fromStdString( metadata.name );
			}
			if ( not metadata.pdbIDCode.empty() )
			{
				return QString::fromStdString( metadata.pdbIDCode );
			}
			if ( not metadata.path.empty() )
			{
				return QString::fromStdString( metadata.path.stem().string() );
			}
			return QString( "System %1" ).arg( static_cast<uint>( p_entity ) );
		}

		QTableWidgetItem * _item( const QString & p_text )
		{
			auto * const item = new QTableWidgetItem( p_text );
			item->setFlags( item->flags() | Qt::ItemIsEditable );
			return item;
		}

		QTableWidgetItem * _readonlyItem( const QString & p_text )
		{
			auto * const item = new QTableWidgetItem( p_text );
			item->setFlags( item->flags() & ~Qt::ItemIsEditable );
			return item;
		}

		void _configureTable( QTableWidget & p_table, const QStringList & p_headers )
		{
			p_table.setColumnCount( p_headers.size() );
			p_table.setHorizontalHeaderLabels( p_headers );
			p_table.setAlternatingRowColors( true );
			p_table.setSelectionBehavior( QAbstractItemView::SelectRows );
			p_table.setSelectionMode( QAbstractItemView::SingleSelection );
			p_table.setEditTriggers(
				QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::EditKeyPressed
			);
			p_table.verticalHeader()->setVisible( false );
			p_table.horizontalHeader()->setStretchLastSection( true );
		}

		void _resizeTableColumns( QTableWidget & p_table )
		{
			for ( int column = 0; column < p_table.columnCount() - 1; ++column )
			{
				p_table.resizeColumnToContents( column );
			}
		}

		std::string_view _atomSymbol( const Core::ChemDB::Atom::SYMBOL p_symbol )
		{
			const size_t index = size_t( p_symbol );
			if ( index >= Core::ChemDB::Atom::SYMBOL_STR.size() )
			{
				return Core::ChemDB::Atom::SYMBOL_STR[ 0 ];
			}
			return Core::ChemDB::Atom::SYMBOL_STR[ index ];
		}

		std::string_view _residueSymbol( const Core::ChemDB::Residue::SYMBOL p_symbol )
		{
			const size_t index = size_t( p_symbol );
			if ( index >= Core::ChemDB::Residue::SYMBOL_STR.size() )
			{
				return Core::ChemDB::Residue::SYMBOL_STR[ 0 ];
			}
			return Core::ChemDB::Residue::SYMBOL_STR[ index ];
		}

		std::string_view _bondOrder( const Core::ChemDB::Bond::ORDER p_order )
		{
			const size_t index = size_t( p_order );
			if ( index >= Core::ChemDB::Bond::ORDER_STR.size() )
			{
				return Core::ChemDB::Bond::ORDER_STR[ 0 ];
			}
			return Core::ChemDB::Bond::ORDER_STR[ index ];
		}
	} // namespace

	TopologyEditorDialog::TopologyEditorDialog() { _buildUi(); }

	void TopologyEditorDialog::showEvent( QShowEvent * p_event )
	{
		UI::QT::Dialog::BaseDialog<TopologyEditorDialog>::showEvent( p_event );
		_refreshSystems();
	}

	void TopologyEditorDialog::_buildUi()
	{
		setWindowTitle( "Topology Editor" );
		setMinimumSize( MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT );
		resize( MIN_DIALOG_WIDTH, MIN_DIALOG_HEIGHT );

		auto * const rootLayout = new QVBoxLayout( this );
		rootLayout->setContentsMargins( 10, 10, 10, 10 );
		rootLayout->setSpacing( 8 );

		auto * const topBar = new QHBoxLayout;
		topBar->setSpacing( 8 );
		auto * const systemLabel = new QLabel( "System", this );
		_systemSelector		  = new QComboBox( this );
		_systemSelector->setMinimumWidth( 320 );
		_systemStatus = new QLabel( this );
		_systemStatus->setMinimumWidth( 220 );
		topBar->addWidget( systemLabel );
		topBar->addWidget( _systemSelector );
		topBar->addWidget( _systemStatus );
		topBar->addStretch();
		rootLayout->addLayout( topBar );

		auto * const mainSplitter = new QSplitter( Qt::Horizontal, this );
		rootLayout->addWidget( mainSplitter, 1 );

		auto * const leftPanel	= new QWidget( mainSplitter );
		auto * const leftLayout = new QVBoxLayout( leftPanel );
		leftLayout->setContentsMargins( 0, 0, 0, 0 );
		leftLayout->setSpacing( 8 );

		auto * const treeGroup	= new QGroupBox( "Structure", leftPanel );
		auto * const treeLayout = new QVBoxLayout( treeGroup );
		_structureTree			= new QTreeView( treeGroup );
		_structureTree->setAlternatingRowColors( true );
		_structureTree->setHeaderHidden( true );
		_structureTree->setEditTriggers( QAbstractItemView::NoEditTriggers );
		_structureTree->setSelectionBehavior( QAbstractItemView::SelectRows );
		_structureTree->setSelectionMode( QAbstractItemView::SingleSelection );
		treeLayout->addWidget( _structureTree );
		leftLayout->addWidget( treeGroup, 1 );

		auto * const actionsGroup	= new QGroupBox( "Operations", leftPanel );
		auto * const actionsLayout = new QVBoxLayout( actionsGroup );
		auto * const toolBar		= new QToolBar( actionsGroup );
		toolBar->setIconSize( QSize( 20, 20 ) );
		toolBar->setToolButtonStyle( Qt::ToolButtonIconOnly );
		toolBar->addAction( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::CHAIN ), "New chain" );
		toolBar->addAction( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::RESIDUE ), "New residue" );
		toolBar->addAction( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::ATOM ), "New atom" );
		toolBar->addSeparator();
		toolBar->addAction( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::DELETE ), "Delete" );
		for ( QAction * const action : toolBar->actions() )
		{
			action->setEnabled( false );
		}
		actionsLayout->addWidget( toolBar );
		leftLayout->addWidget( actionsGroup );

		auto * const centerTabs = new QTabWidget( mainSplitter );
		_chainsTable		   = new QTableWidget( centerTabs );
		_residuesTable		   = new QTableWidget( centerTabs );
		_atomsTable			   = new QTableWidget( centerTabs );
		_bondsTable			   = new QTableWidget( centerTabs );
		_validationTable	   = new QTableWidget( centerTabs );

		_configureTable( *_chainsTable, { "Name", "Residues", "Atoms" } );
		_configureTable( *_residuesTable, { "Chain", "Name", "Symbol", "Original ID", "Atoms" } );
		_configureTable( *_atomsTable, { "Residue", "Name", "Element", "X", "Y", "Z" } );
		_configureTable( *_bondsTable, { "Atom A", "Atom B", "Order" } );
		_configureTable( *_validationTable, { "Severity", "Item", "Message" } );
		_validationTable->setEditTriggers( QAbstractItemView::NoEditTriggers );

		centerTabs->addTab( _chainsTable, "Chains" );
		centerTabs->addTab( _residuesTable, "Residues" );
		centerTabs->addTab( _atomsTable, "Atoms" );
		centerTabs->addTab( _bondsTable, "Bonds" );
		centerTabs->addTab( _validationTable, "Validation" );

		auto * const rightPanel	  = new QWidget( mainSplitter );
		auto * const rightLayout = new QVBoxLayout( rightPanel );
		rightLayout->setContentsMargins( 0, 0, 0, 0 );
		rightLayout->setSpacing( 8 );

		auto * const inspectorGroup	= new QGroupBox( "Selection", rightPanel );
		auto * const inspectorLayout = new QFormLayout( inspectorGroup );
		_selectedType				= new QLineEdit( inspectorGroup );
		_selectedName				= new QLineEdit( inspectorGroup );
		_selectedParent				= new QLineEdit( inspectorGroup );
		_selectedPosition			= new QLineEdit( inspectorGroup );
		for ( QLineEdit * const edit : { _selectedType.data(), _selectedName.data(), _selectedParent.data(), _selectedPosition.data() } )
		{
			edit->setReadOnly( true );
		}
		inspectorLayout->addRow( "Type", _selectedType );
		inspectorLayout->addRow( "Name", _selectedName );
		inspectorLayout->addRow( "Parent", _selectedParent );
		inspectorLayout->addRow( "Position", _selectedPosition );
		rightLayout->addWidget( inspectorGroup );

		auto * const topologyGroup  = new QGroupBox( "Topology", rightPanel );
		auto * const topologyGrid	= new QGridLayout( topologyGroup );
		topologyGrid->addWidget( new QLabel( "Chains", topologyGroup ), 0, 0 );
		topologyGrid->addWidget( new QLabel( "Residues", topologyGroup ), 1, 0 );
		topologyGrid->addWidget( new QLabel( "Atoms", topologyGroup ), 2, 0 );
		topologyGrid->addWidget( new QLabel( "Bonds", topologyGroup ), 3, 0 );
		topologyGrid->addWidget( new QLabel( "-", topologyGroup ), 0, 1 );
		topologyGrid->addWidget( new QLabel( "-", topologyGroup ), 1, 1 );
		topologyGrid->addWidget( new QLabel( "-", topologyGroup ), 2, 1 );
		topologyGrid->addWidget( new QLabel( "-", topologyGroup ), 3, 1 );
		topologyGroup->setProperty( "vtxTopologyCountGrid", true );
		rightLayout->addWidget( topologyGroup );
		rightLayout->addStretch();

		mainSplitter->addWidget( leftPanel );
		mainSplitter->addWidget( centerTabs );
		mainSplitter->addWidget( rightPanel );
		mainSplitter->setStretchFactor( 0, 2 );
		mainSplitter->setStretchFactor( 1, 5 );
		mainSplitter->setStretchFactor( 2, 2 );

		auto * const buttons = new QDialogButtonBox( this );
		_applyButton		= buttons->addButton( QDialogButtonBox::Apply );
		_applyButton->setIcon( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::APPLY ) );
		_applyButton->setEnabled( false );
		auto * const closeButton = buttons->addButton( QDialogButtonBox::Close );
		closeButton->setIcon( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::CANCEL ) );
		rootLayout->addWidget( buttons );

		connect( closeButton, &QPushButton::clicked, this, &QDialog::reject );
		connect(
			_systemSelector,
			&QComboBox::currentIndexChanged,
			this,
			[ this ]( const int p_index )
			{
				if ( p_index < 0 )
				{
					_clearSystemViews();
					return;
				}
				_setCurrentSystem( App::ECS::Entity( _systemSelector->itemData( p_index ).toUInt() ) );
			}
		);
		connect(
			_structureTree,
			&QTreeView::clicked,
			this,
			[ this ]( const QModelIndex & p_index ) { _updateSelectionInspector( p_index ); }
		);

		_clearSystemViews();
	}

	void TopologyEditorDialog::_refreshSystems()
	{
		const QVariant previous
			= _systemSelector->currentIndex() >= 0 ? _systemSelector->currentData() : QVariant();

		_systemSelector->blockSignals( true );
		_systemSelector->clear();

		auto view = App::REG().view<Core::Struct::Topology, App::System::Metadata>();
		for ( const App::ECS::Entity entity : view )
		{
			_systemSelector->addItem( _entityLabel( entity ), static_cast<uint>( entity ) );
		}

		int indexToSelect = 0;
		if ( previous.isValid() )
		{
			const int previousIndex = _systemSelector->findData( previous );
			if ( previousIndex >= 0 )
			{
				indexToSelect = previousIndex;
			}
		}

		_systemSelector->blockSignals( false );

		if ( _systemSelector->count() == 0 )
		{
			_clearSystemViews();
			return;
		}

		_systemSelector->setCurrentIndex( indexToSelect );
		_setCurrentSystem( App::ECS::Entity( _systemSelector->itemData( indexToSelect ).toUInt() ) );
	}

	void TopologyEditorDialog::_setCurrentSystem( const App::ECS::Entity p_system )
	{
		if ( not App::REG().valid( p_system ) || not App::REG().all_of<Core::Struct::Topology>( p_system ) )
		{
			_clearSystemViews();
			return;
		}

		_currentSystem = p_system;
		_systemModel	= std::make_unique<UI::QT::Model::SystemModel>( p_system );
		_structureTree->setModel( _systemModel.get() );
		_structureTree->expandToDepth( 1 );
		_populateTables();
	}

	void TopologyEditorDialog::_clearSystemViews()
	{
		_currentSystem.reset();
		_systemModel.reset();

		if ( _structureTree )
		{
			_structureTree->setModel( nullptr );
		}
		for ( QTableWidget * const table :
			  { _chainsTable.data(), _residuesTable.data(), _atomsTable.data(), _bondsTable.data(), _validationTable.data() } )
		{
			if ( table )
			{
				table->setRowCount( 0 );
			}
		}
		if ( _selectedType )
		{
			_selectedType->clear();
			_selectedName->clear();
			_selectedParent->clear();
			_selectedPosition->clear();
		}
		if ( _systemStatus )
		{
			_systemStatus->setText( "No system" );
		}
		if ( _applyButton )
		{
			_applyButton->setEnabled( false );
		}
	}

	void TopologyEditorDialog::_populateTables()
	{
		_populateChainsTable();
		_populateResiduesTable();
		_populateAtomsTable();
		_populateBondsTable();
		_populateValidationTable();

		const auto & topology = App::REG().get<Core::Struct::Topology>( *_currentSystem );
		_systemStatus->setText(
			QString( "%1 chains | %2 residues | %3 atoms" )
				.arg( topology.getChainCount() )
				.arg( topology.getResidueCount() )
				.arg( topology.getAtomCount() )
		);
	}

	void TopologyEditorDialog::_populateChainsTable()
	{
		const auto & topology = App::REG().get<Core::Struct::Topology>( *_currentSystem );
		_chainsTable->setRowCount( int( topology.getChainCount() ) );
		for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
		{
			_chainsTable->setItem( int( chain ), 0, _item( _textOrDash( topology.getChainName( chain ) ) ) );
			_chainsTable->setItem( int( chain ), 1, _readonlyItem( QString::number( topology.getChainResidueCount( chain ) ) ) );
			_chainsTable->setItem( int( chain ), 2, _readonlyItem( QString::number( topology.getChainAtomCount( chain ) ) ) );
		}
		_resizeTableColumns( *_chainsTable );
	}

	void TopologyEditorDialog::_populateResiduesTable()
	{
		const auto & topology = App::REG().get<Core::Struct::Topology>( *_currentSystem );
		_residuesTable->setRowCount( int( topology.getResidueCount() ) );
		for ( Index residue = 0; residue < topology.getResidueCount(); ++residue )
		{
			const Index chain = topology.getResidueChainIndex( residue );
			_residuesTable->setItem( int( residue ), 0, _readonlyItem( _textOrDash( topology.getChainName( chain ) ) ) );
			_residuesTable->setItem( int( residue ), 1, _item( _textOrDash( topology.getResidueName( residue ) ) ) );
			_residuesTable->setItem(
				int( residue ),
				2,
				_item( QString::fromStdString( std::string( _residueSymbol( topology.getResidueSymbol( residue ) ) ) ) )
			);
			_residuesTable->setItem(
				int( residue ), 3, _item( QString::number( topology.getResidueOriginalId( residue ) ) )
			);
			_residuesTable->setItem(
				int( residue ), 4, _readonlyItem( QString::number( topology.getResidueAtomCount( residue ) ) )
			);
		}
		_resizeTableColumns( *_residuesTable );
	}

	void TopologyEditorDialog::_populateAtomsTable()
	{
		const auto & topology  = App::REG().get<Core::Struct::Topology>( *_currentSystem );
		const auto	positions = App::System::getCurrentAtomPositions( *_currentSystem );
		_atomsTable->setRowCount( int( topology.getAtomCount() ) );
		for ( Index atom = 0; atom < topology.getAtomCount(); ++atom )
		{
			const Index residue = topology.getAtomResidueIndex( atom );
			_atomsTable->setItem( int( atom ), 0, _readonlyItem( _textOrDash( topology.getResidueName( residue ) ) ) );
			_atomsTable->setItem( int( atom ), 1, _item( _textOrDash( topology.getAtomName( atom ) ) ) );
			_atomsTable->setItem(
				int( atom ),
				2,
				_item( QString::fromStdString( std::string( _atomSymbol( topology.getAtomSymbol( atom ) ) ) ) )
			);

			const bool hasPosition = atom < positions.size();
			const Vec3f position	 = hasPosition ? positions[ atom ] : Vec3f {};
			_atomsTable->setItem( int( atom ), 3, _item( hasPosition ? QString::number( position.x ) : "-" ) );
			_atomsTable->setItem( int( atom ), 4, _item( hasPosition ? QString::number( position.y ) : "-" ) );
			_atomsTable->setItem( int( atom ), 5, _item( hasPosition ? QString::number( position.z ) : "-" ) );
		}
		_resizeTableColumns( *_atomsTable );
	}

	void TopologyEditorDialog::_populateBondsTable()
	{
		const auto & topology = App::REG().get<Core::Struct::Topology>( *_currentSystem );
		_bondsTable->setRowCount( int( topology.getBondCount() ) );
		for ( Index bond = 0; bond < topology.getBondCount(); ++bond )
		{
			const Index firstAtom	= topology.getBondFirstAtom( bond );
			const Index secondAtom = topology.getBondSecondAtom( bond );
			_bondsTable->setItem( int( bond ), 0, _readonlyItem( QString::number( firstAtom ) ) );
			_bondsTable->setItem( int( bond ), 1, _readonlyItem( QString::number( secondAtom ) ) );
			_bondsTable->setItem(
				int( bond ),
				2,
				_item( QString::fromStdString( std::string( _bondOrder( topology.getBondOrder( bond ) ) ) ) )
			);
		}
		_resizeTableColumns( *_bondsTable );
	}

	void TopologyEditorDialog::_populateValidationTable()
	{
		const auto & topology  = App::REG().get<Core::Struct::Topology>( *_currentSystem );
		const auto	positions = App::System::getCurrentAtomPositions( *_currentSystem );

		_validationTable->setRowCount( 0 );
		auto addIssue = [ this ]( const QString & p_severity, const QString & p_item, const QString & p_message )
		{
			const int row = _validationTable->rowCount();
			_validationTable->insertRow( row );
			_validationTable->setItem( row, 0, _readonlyItem( p_severity ) );
			_validationTable->setItem( row, 1, _readonlyItem( p_item ) );
			_validationTable->setItem( row, 2, _readonlyItem( p_message ) );
		};

		if ( topology.getAtomCount() == 0 )
		{
			addIssue( "Error", "System", "Empty topology" );
		}
		if ( positions.size() != topology.getAtomCount() )
		{
			addIssue( "Error", "Positions", "Atom count mismatch" );
		}
		for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
		{
			if ( topology.getChainResidueCount( chain ) == 0 )
			{
				addIssue( "Warning", QString( "Chain %1" ).arg( chain ), "Empty chain" );
			}
		}
		if ( _validationTable->rowCount() == 0 )
		{
			addIssue( "Info", "System", "Ready" );
		}
		_resizeTableColumns( *_validationTable );
	}

	void TopologyEditorDialog::_updateSelectionInspector( const QModelIndex & p_index )
	{
		if ( not p_index.isValid() || not _currentSystem )
		{
			return;
		}

		Core::Struct::E_SYSTEM_ITEM item;
		Index					   index;
		UI::QT::Model::SystemModel::unpack( p_index.internalId(), item, index );

		const auto & topology  = App::REG().get<Core::Struct::Topology>( *_currentSystem );
		const auto	positions = App::System::getCurrentAtomPositions( *_currentSystem );

		_selectedParent->clear();
		_selectedPosition->clear();

		switch ( item )
		{
		case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
			_selectedType->setText( "System" );
			_selectedName->setText( _entityLabel( *_currentSystem ) );
			break;
		case Core::Struct::E_SYSTEM_ITEM::CHAIN:
			_selectedType->setText( "Chain" );
			_selectedName->setText( _textOrDash( topology.getChainName( index ) ) );
			_selectedParent->setText( _entityLabel( *_currentSystem ) );
			break;
		case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
		{
			const Index chain = topology.getResidueChainIndex( index );
			_selectedType->setText( "Residue" );
			_selectedName->setText( _textOrDash( topology.getResidueName( index ) ) );
			_selectedParent->setText( _textOrDash( topology.getChainName( chain ) ) );
			break;
		}
		case Core::Struct::E_SYSTEM_ITEM::ATOM:
		{
			const Index residue = topology.getAtomResidueIndex( index );
			_selectedType->setText( "Atom" );
			_selectedName->setText( _textOrDash( topology.getAtomName( index ) ) );
			_selectedParent->setText( _textOrDash( topology.getResidueName( residue ) ) );
			if ( index < positions.size() )
			{
				const Vec3f position = positions[ index ];
				_selectedPosition->setText(
					QString( "%1, %2, %3" ).arg( position.x ).arg( position.y ).arg( position.z )
				);
			}
			break;
		}
		}
	}
} // namespace VTX::Tool::TopologyEditor::Dialog
