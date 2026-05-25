#include "tool/topology_editor/dialog/topology_editor_dialog.hpp"
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QShowEvent>
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <app/services.hpp>
#include <app/system/trajectory.hpp>
#include <core/chemdb/atom.hpp>
#include <core/chemdb/bond.hpp>
#include <core/chemdb/residue.hpp>
#include <core/struct/topology.hpp>
#include <io/metadata.hpp>
#include <ui/qt/helper.hpp>
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

		QString _entityLabel( const Entity p_entity )
		{
			const auto & metadata = App::REG().get<IO::Metadata>( p_entity );
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
			item->setFlags( item->flags() & ~Qt::ItemIsEditable );
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
			p_table.setEditTriggers( QAbstractItemView::NoEditTriggers );
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

	TopologyEditorDialog::TopologyEditorDialog( const Entity p_system ) : _system( p_system ) { _buildUi(); }

	Entity TopologyEditorDialog::getSystem() const { return _system; }

	void TopologyEditorDialog::showEvent( QShowEvent * p_event )
	{
		UI::QT::Dialog::BaseDialog<TopologyEditorDialog>::showEvent( p_event );
		if ( App::REG().valid( _system ) && App::REG().all_of<Core::Struct::Topology>( _system ) )
		{
			_populateTables();
		}
		else
		{
			_clearSystemViews();
		}
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
		_systemName				 = new QLabel( this );
		_systemName->setMinimumWidth( 320 );
		_systemStatus = new QLabel( this );
		_systemStatus->setMinimumWidth( 220 );
		topBar->addWidget( systemLabel );
		topBar->addWidget( _systemName, 1 );
		topBar->addWidget( _systemStatus );
		topBar->addStretch();
		rootLayout->addLayout( topBar );

		auto * const mainSplitter = new QSplitter( Qt::Horizontal, this );
		rootLayout->addWidget( mainSplitter, 1 );

		auto * const centerPanel  = new QWidget( mainSplitter );
		auto * const centerLayout = new QVBoxLayout( centerPanel );
		centerLayout->setContentsMargins( 0, 0, 0, 0 );
		centerLayout->setSpacing( 8 );

		auto * const centerTabs	  = new QTabWidget( centerPanel );
		auto * const chainsPage	  = new QWidget( centerTabs );
		auto * const chainsLayout = new QVBoxLayout( chainsPage );
		chainsLayout->setContentsMargins( 0, 0, 0, 0 );
		chainsLayout->setSpacing( 6 );
		_chainsFilter = new QLineEdit( chainsPage );
		_chainsFilter->setClearButtonEnabled( true );
		_chainsFilter->setPlaceholderText( "Filter chains" );
		_chainsTable = new QTableWidget( chainsPage );
		_configureTable( *_chainsTable, { "Name", "Residues", "Atoms" } );
		chainsLayout->addWidget( _chainsFilter );
		chainsLayout->addWidget( _chainsTable, 1 );

		auto * const residuesPage	= new QWidget( centerTabs );
		auto * const residuesLayout = new QVBoxLayout( residuesPage );
		residuesLayout->setContentsMargins( 0, 0, 0, 0 );
		residuesLayout->setSpacing( 6 );
		_residuesFilter = new QLineEdit( residuesPage );
		_residuesFilter->setClearButtonEnabled( true );
		_residuesFilter->setPlaceholderText( "Filter residues" );
		_residuesTable = new QTableWidget( residuesPage );
		_configureTable(
			*_residuesTable, { "Index", "Chain", "Name", "Symbol", "Original ID", "Secondary Structure", "Atoms" }
		);
		residuesLayout->addWidget( _residuesFilter );
		residuesLayout->addWidget( _residuesTable, 1 );

		auto * const atomsPage	 = new QWidget( centerTabs );
		auto * const atomsLayout = new QVBoxLayout( atomsPage );
		atomsLayout->setContentsMargins( 0, 0, 0, 0 );
		atomsLayout->setSpacing( 6 );
		_atomsFilter = new QLineEdit( atomsPage );
		_atomsFilter->setClearButtonEnabled( true );
		_atomsFilter->setPlaceholderText( "Filter atoms" );
		_atomsTable = new QTableWidget( atomsPage );
		_configureTable(
			*_atomsTable, { "Index", "Residue", "In Residue", "Name", "Original Index", "Element", "X", "Y", "Z" }
		);
		atomsLayout->addWidget( _atomsFilter );
		atomsLayout->addWidget( _atomsTable, 1 );

		auto * const bondsPage	 = new QWidget( centerTabs );
		auto * const bondsLayout = new QVBoxLayout( bondsPage );
		bondsLayout->setContentsMargins( 0, 0, 0, 0 );
		bondsLayout->setSpacing( 6 );
		_bondsFilter = new QLineEdit( bondsPage );
		_bondsFilter->setClearButtonEnabled( true );
		_bondsFilter->setPlaceholderText( "Filter bonds" );
		_bondsTable = new QTableWidget( bondsPage );
		_configureTable( *_bondsTable, { "Atom A", "Atom B", "Order" } );
		bondsLayout->addWidget( _bondsFilter );
		bondsLayout->addWidget( _bondsTable, 1 );

		auto * const validationPage	  = new QWidget( centerTabs );
		auto * const validationLayout = new QVBoxLayout( validationPage );
		validationLayout->setContentsMargins( 0, 0, 0, 0 );
		validationLayout->setSpacing( 6 );
		_validationFilter = new QLineEdit( validationPage );
		_validationFilter->setClearButtonEnabled( true );
		_validationFilter->setPlaceholderText( "Filter validation" );
		_validationTable = new QTableWidget( validationPage );
		_configureTable( *_validationTable, { "Severity", "Item", "Message" } );
		validationLayout->addWidget( _validationFilter );
		validationLayout->addWidget( _validationTable, 1 );
		_validationTable->setEditTriggers( QAbstractItemView::NoEditTriggers );

		centerTabs->addTab( chainsPage, "Chains" );
		centerTabs->addTab( residuesPage, "Residues" );
		centerTabs->addTab( atomsPage, "Atoms" );
		centerTabs->addTab( bondsPage, "Bonds" );
		centerTabs->addTab( validationPage, "Validation" );
		centerLayout->addWidget( centerTabs, 1 );

		auto * const rightPanel	 = new QWidget( mainSplitter );
		auto * const rightLayout = new QVBoxLayout( rightPanel );
		rightLayout->setContentsMargins( 0, 0, 0, 0 );
		rightLayout->setSpacing( 8 );

		auto * const inspectorGroup	 = new QGroupBox( "Selection", rightPanel );
		auto * const inspectorLayout = new QFormLayout( inspectorGroup );
		_selectedType				 = new QLineEdit( inspectorGroup );
		_selectedName				 = new QLineEdit( inspectorGroup );
		for ( QLineEdit * const edit : { _selectedType.data(), _selectedName.data() } )
		{
			edit->setReadOnly( true );
		}
		inspectorLayout->addRow( "Type", _selectedType );
		inspectorLayout->addRow( "Name", _selectedName );
		for ( int propertyIndex = 0; propertyIndex < int( _propertyLabels.size() ); ++propertyIndex )
		{
			_propertyLabels[ propertyIndex ] = new QLabel( inspectorGroup );
			_propertyValues[ propertyIndex ] = new QLineEdit( inspectorGroup );
			_propertyValues[ propertyIndex ]->setReadOnly( true );
			inspectorLayout->addRow( _propertyLabels[ propertyIndex ], _propertyValues[ propertyIndex ] );
		}
		rightLayout->addWidget( inspectorGroup );
		rightLayout->addStretch();

		mainSplitter->addWidget( centerPanel );
		mainSplitter->addWidget( rightPanel );
		mainSplitter->setStretchFactor( 0, 6 );
		mainSplitter->setStretchFactor( 1, 3 );

		auto * const buttons = new QDialogButtonBox( this );
		_applyButton		 = buttons->addButton( QDialogButtonBox::Apply );
		_applyButton->setIcon( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::APPLY ) );
		_applyButton->setEnabled( false );
		auto * const closeButton = buttons->addButton( QDialogButtonBox::Close );
		closeButton->setIcon( UI::QT::STYLE().iconFromCodepoint( UI::QT::Style::Icons::CANCEL ) );
		rootLayout->addWidget( buttons );

		connect( closeButton, &QPushButton::clicked, this, &QDialog::reject );
		connect(
			_chainsTable,
			&QTableWidget::currentCellChanged,
			this,
			[ this ]( const int p_row, const int, const int, const int )
			{
				if ( p_row < 0 )
				{
					_clearSelectionInspector();
					return;
				}
				_updateSelectionInspector( Core::Struct::E_SYSTEM_ITEM::CHAIN, Index( p_row ) );
			}
		);
		connect(
			_residuesTable,
			&QTableWidget::currentCellChanged,
			this,
			[ this ]( const int p_row, const int, const int, const int )
			{
				if ( p_row < 0 )
				{
					_clearSelectionInspector();
					return;
				}
				_updateSelectionInspector( Core::Struct::E_SYSTEM_ITEM::RESIDUE, Index( p_row ) );
			}
		);
		connect(
			_atomsTable,
			&QTableWidget::currentCellChanged,
			this,
			[ this ]( const int p_row, const int, const int, const int )
			{
				if ( p_row < 0 )
				{
					_clearSelectionInspector();
					return;
				}
				_updateSelectionInspector( Core::Struct::E_SYSTEM_ITEM::ATOM, Index( p_row ) );
			}
		);
		connect(
			_bondsTable,
			&QTableWidget::currentCellChanged,
			this,
			[ this ]( const int p_row, const int, const int, const int )
			{
				if ( p_row < 0 )
				{
					_clearSelectionInspector();
					return;
				}
				_updateBondInspector( Index( p_row ) );
			}
		);
		connect(
			_validationTable,
			&QTableWidget::currentCellChanged,
			this,
			[ this ]( const int p_row, const int, const int, const int )
			{
				if ( p_row < 0 )
				{
					return;
				}
				_clearSelectionInspector();
			}
		);
		connect(
			_chainsFilter,
			&QLineEdit::textChanged,
			this,
			[ this ]( const QString & p_text ) { _applyTableFilter( *_chainsTable, p_text ); }
		);
		connect(
			_residuesFilter,
			&QLineEdit::textChanged,
			this,
			[ this ]( const QString & p_text ) { _applyTableFilter( *_residuesTable, p_text ); }
		);
		connect(
			_atomsFilter,
			&QLineEdit::textChanged,
			this,
			[ this ]( const QString & p_text ) { _applyTableFilter( *_atomsTable, p_text ); }
		);
		connect(
			_bondsFilter,
			&QLineEdit::textChanged,
			this,
			[ this ]( const QString & p_text ) { _applyTableFilter( *_bondsTable, p_text ); }
		);
		connect(
			_validationFilter,
			&QLineEdit::textChanged,
			this,
			[ this ]( const QString & p_text ) { _applyTableFilter( *_validationTable, p_text ); }
		);

		_clearSystemViews();
	}

	void TopologyEditorDialog::_clearSystemViews()
	{
		for ( QTableWidget * const table : { _chainsTable.data(),
											 _residuesTable.data(),
											 _atomsTable.data(),
											 _bondsTable.data(),
											 _validationTable.data() } )
		{
			if ( table )
			{
				table->setRowCount( 0 );
			}
		}
		for ( QLineEdit * const filter : { _chainsFilter.data(),
										   _residuesFilter.data(),
										   _atomsFilter.data(),
										   _bondsFilter.data(),
										   _validationFilter.data() } )
		{
			if ( filter )
			{
				filter->clear();
			}
		}
		_clearSelectionInspector();
		if ( _systemStatus )
		{
			_systemStatus->setText( "No system" );
		}
		if ( _systemName )
		{
			_systemName->setText( "-" );
		}
		if ( _applyButton )
		{
			_applyButton->setEnabled( false );
		}
	}

	void TopologyEditorDialog::_clearSelectionInspector()
	{
		if ( _selectedType )
		{
			_selectedType->clear();
			_selectedName->clear();
		}

		for ( int propertyIndex = 0; propertyIndex < int( _propertyLabels.size() ); ++propertyIndex )
		{
			if ( _propertyLabels[ propertyIndex ] )
			{
				_propertyLabels[ propertyIndex ]->clear();
				_propertyLabels[ propertyIndex ]->setVisible( false );
			}
			if ( _propertyValues[ propertyIndex ] )
			{
				_propertyValues[ propertyIndex ]->clear();
				_propertyValues[ propertyIndex ]->setVisible( false );
			}
		}
	}

	void TopologyEditorDialog::_setInspectorProperty(
		const int		p_index,
		const QString & p_label,
		const QString & p_value
	)
	{
		if ( p_index < 0 || p_index >= int( _propertyLabels.size() ) )
		{
			return;
		}

		_propertyLabels[ p_index ]->setText( p_label );
		_propertyLabels[ p_index ]->setVisible( true );
		_propertyValues[ p_index ]->setText( p_value );
		_propertyValues[ p_index ]->setVisible( true );
	}

	void TopologyEditorDialog::_populateTables()
	{
		const auto & topology = App::REG().get<Core::Struct::Topology>( _system );
		if ( _systemName )
		{
			_systemName->setText( _entityLabel( _system ) );
		}

		_populateChainsTable();
		_populateResiduesTable();
		_populateAtomsTable();
		_populateBondsTable();
		_populateValidationTable();

		_systemStatus->setText( QString( "%1 chains | %2 residues | %3 atoms | %4 bonds" )
									.arg( UI::QT::Helper::formatNumber( topology.getChainCount() ) )
									.arg( UI::QT::Helper::formatNumber( topology.getResidueCount() ) )
									.arg( UI::QT::Helper::formatNumber( topology.getAtomCount() ) )
									.arg( UI::QT::Helper::formatNumber( topology.getBondCount() ) ) );
	}

	void TopologyEditorDialog::_populateChainsTable()
	{
		const auto & topology = App::REG().get<Core::Struct::Topology>( _system );
		_chainsTable->setRowCount( int( topology.getChainCount() ) );
		for ( Index chain = 0; chain < topology.getChainCount(); ++chain )
		{
			_chainsTable->setItem( int( chain ), 0, _item( _textOrDash( topology.getChainName( chain ) ) ) );
			_chainsTable->setItem(
				int( chain ), 1, _readonlyItem( UI::QT::Helper::formatNumber( topology.getChainResidueCount( chain ) ) )
			);
			_chainsTable->setItem(
				int( chain ), 2, _readonlyItem( UI::QT::Helper::formatNumber( topology.getChainAtomCount( chain ) ) )
			);
		}
		_resizeTableColumns( *_chainsTable );
		_applyTableFilter( *_chainsTable, _chainsFilter ? _chainsFilter->text() : QString() );
	}

	void TopologyEditorDialog::_populateResiduesTable()
	{
		const auto & topology = App::REG().get<Core::Struct::Topology>( _system );
		_residuesTable->setRowCount( int( topology.getResidueCount() ) );
		for ( Index residue = 0; residue < topology.getResidueCount(); ++residue )
		{
			const Index chain = topology.getResidueChainIndex( residue );
			_residuesTable->setItem( int( residue ), 0, _readonlyItem( UI::QT::Helper::formatNumber( residue ) ) );
			_residuesTable->setItem(
				int( residue ), 1, _readonlyItem( _textOrDash( topology.getChainName( chain ) ) )
			);
			_residuesTable->setItem( int( residue ), 2, _item( _textOrDash( topology.getResidueName( residue ) ) ) );
			_residuesTable->setItem(
				int( residue ),
				3,
				_item( QString::fromStdString( std::string( _residueSymbol( topology.getResidueSymbol( residue ) ) ) ) )
			);
			_residuesTable->setItem(
				int( residue ), 4, _item( QString::number( topology.getResidueOriginalId( residue ) ) )
			);
			_residuesTable->setItem(
				int( residue ),
				5,
				_readonlyItem( QString::fromStdString(
					std::string( Util::Enum::enumName( topology.getResidueSecondaryStructureType( residue ) ) )
				) )
			);
			_residuesTable->setItem(
				int( residue ),
				6,
				_readonlyItem( UI::QT::Helper::formatNumber( topology.getResidueAtomCount( residue ) ) )
			);
		}
		_resizeTableColumns( *_residuesTable );
		_applyTableFilter( *_residuesTable, _residuesFilter ? _residuesFilter->text() : QString() );
	}

	void TopologyEditorDialog::_populateAtomsTable()
	{
		const auto & topology  = App::REG().get<Core::Struct::Topology>( _system );
		const auto	 positions = App::System::getCurrentAtomPositions( _system );
		_atomsTable->setRowCount( int( topology.getAtomCount() ) );
		for ( Index atom = 0; atom < topology.getAtomCount(); ++atom )
		{
			const Index residue		  = topology.getAtomResidueIndex( atom );
			const Index atomInResidue = atom - topology.getResidueFirstAtom( residue ) + 1;
			_atomsTable->setItem( int( atom ), 0, _readonlyItem( UI::QT::Helper::formatNumber( atom ) ) );
			_atomsTable->setItem( int( atom ), 1, _readonlyItem( _textOrDash( topology.getResidueName( residue ) ) ) );
			_atomsTable->setItem( int( atom ), 2, _readonlyItem( UI::QT::Helper::formatNumber( atomInResidue ) ) );
			_atomsTable->setItem( int( atom ), 3, _item( _textOrDash( topology.getAtomName( atom ) ) ) );
			const auto originalAtomIndex = topology.getAtomOriginalIndex( atom );
			_atomsTable->setItem(
				int( atom ),
				4,
				_item( originalAtomIndex ? QString::number( *originalAtomIndex ) : "-" )
			);
			_atomsTable->setItem(
				int( atom ),
				5,
				_item( QString::fromStdString( std::string( _atomSymbol( topology.getAtomSymbol( atom ) ) ) ) )
			);

			const bool	hasPosition = atom < positions.size();
			const Vec3f position	= hasPosition ? positions[ atom ] : Vec3f {};
			_atomsTable->setItem( int( atom ), 6, _item( hasPosition ? QString::number( position.x ) : "-" ) );
			_atomsTable->setItem( int( atom ), 7, _item( hasPosition ? QString::number( position.y ) : "-" ) );
			_atomsTable->setItem( int( atom ), 8, _item( hasPosition ? QString::number( position.z ) : "-" ) );
		}
		_resizeTableColumns( *_atomsTable );
		_applyTableFilter( *_atomsTable, _atomsFilter ? _atomsFilter->text() : QString() );
	}

	void TopologyEditorDialog::_populateBondsTable()
	{
		const auto & topology = App::REG().get<Core::Struct::Topology>( _system );
		_bondsTable->setRowCount( int( topology.getBondCount() ) );
		for ( Index bond = 0; bond < topology.getBondCount(); ++bond )
		{
			const Index firstAtom  = topology.getBondFirstAtom( bond );
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
		_applyTableFilter( *_bondsTable, _bondsFilter ? _bondsFilter->text() : QString() );
	}

	void TopologyEditorDialog::_populateValidationTable()
	{
		const auto & topology  = App::REG().get<Core::Struct::Topology>( _system );
		const auto	 positions = App::System::getCurrentAtomPositions( _system );

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
		_applyTableFilter( *_validationTable, _validationFilter ? _validationFilter->text() : QString() );
	}

	void TopologyEditorDialog::_applyTableFilter( QTableWidget & p_table, const QString & p_filter )
	{
		const QString normalizedFilter = p_filter.trimmed();
		for ( int row = 0; row < p_table.rowCount(); ++row )
		{
			bool visible = normalizedFilter.isEmpty();
			if ( not visible )
			{
				visible = QString::number( row ).contains( normalizedFilter, Qt::CaseInsensitive );
			}
			if ( not visible )
			{
				for ( int column = 0; column < p_table.columnCount(); ++column )
				{
					const QTableWidgetItem * const item = p_table.item( row, column );
					if ( item != nullptr && item->text().contains( normalizedFilter, Qt::CaseInsensitive ) )
					{
						visible = true;
						break;
					}
				}
			}

			p_table.setRowHidden( row, not visible );
		}
	}

	void TopologyEditorDialog::_updateSelectionInspector(
		const Core::Struct::E_SYSTEM_ITEM p_item,
		const Index						  p_index
	)
	{
		const auto & topology  = App::REG().get<Core::Struct::Topology>( _system );
		const auto	 positions = App::System::getCurrentAtomPositions( _system );

		_clearSelectionInspector();

		switch ( p_item )
		{
		case Core::Struct::E_SYSTEM_ITEM::SYSTEM:
			_selectedType->setText( "System" );
			_selectedName->setText( _entityLabel( _system ) );
			_setInspectorProperty( 0, "Chains", UI::QT::Helper::formatNumber( topology.getChainCount() ) );
			_setInspectorProperty( 1, "Residues", UI::QT::Helper::formatNumber( topology.getResidueCount() ) );
			_setInspectorProperty( 2, "Atoms", UI::QT::Helper::formatNumber( topology.getAtomCount() ) );
			_setInspectorProperty( 3, "Bonds", UI::QT::Helper::formatNumber( topology.getBondCount() ) );
			break;
		case Core::Struct::E_SYSTEM_ITEM::CHAIN:
			_selectedType->setText( "Chain" );
			_selectedName->setText( _textOrDash( topology.getChainName( p_index ) ) );
			_setInspectorProperty( 0, "System", _entityLabel( _system ) );
			_setInspectorProperty(
				1, "Residues", UI::QT::Helper::formatNumber( topology.getChainResidueCount( p_index ) )
			);
			_setInspectorProperty( 2, "Atoms", UI::QT::Helper::formatNumber( topology.getChainAtomCount( p_index ) ) );
			_setInspectorProperty( 3, "Bonds", UI::QT::Helper::formatNumber( topology.getChainBondCount( p_index ) ) );
			break;
		case Core::Struct::E_SYSTEM_ITEM::RESIDUE:
		{
			const Index chain = topology.getResidueChainIndex( p_index );
			_selectedType->setText( "Residue" );
			_selectedName->setText( _textOrDash( topology.getResidueName( p_index ) ) );
			_setInspectorProperty( 0, "Chain", _textOrDash( topology.getChainName( chain ) ) );
			_setInspectorProperty(
				1,
				"Symbol",
				QString::fromStdString( std::string( _residueSymbol( topology.getResidueSymbol( p_index ) ) ) )
			);
			_setInspectorProperty(
				2, "Original ID", UI::QT::Helper::formatNumber( topology.getResidueOriginalId( p_index ) )
			);
			_setInspectorProperty(
				3, "Atoms", UI::QT::Helper::formatNumber( topology.getResidueAtomCount( p_index ) )
			);
			_setInspectorProperty(
				4, "Bonds", UI::QT::Helper::formatNumber( topology.getResidueBondCount( p_index ) )
			);
			break;
		}
		case Core::Struct::E_SYSTEM_ITEM::ATOM:
		{
			const Index residue = topology.getAtomResidueIndex( p_index );
			const Index chain	= topology.getResidueChainIndex( residue );
			_selectedType->setText( "Atom" );
			_selectedName->setText( _textOrDash( topology.getAtomName( p_index ) ) );
			_setInspectorProperty( 0, "Chain", _textOrDash( topology.getChainName( chain ) ) );
			_setInspectorProperty( 1, "Residue", _textOrDash( topology.getResidueName( residue ) ) );
			_setInspectorProperty(
				2, "Element", QString::fromStdString( std::string( _atomSymbol( topology.getAtomSymbol( p_index ) ) ) )
			);
			if ( p_index < positions.size() )
			{
				const Vec3f position = positions[ p_index ];
				_setInspectorProperty( 3, "X", UI::QT::Helper::formatNumber( position.x, 'f', 3 ) );
				_setInspectorProperty( 4, "Y", UI::QT::Helper::formatNumber( position.y, 'f', 3 ) );
				_setInspectorProperty( 5, "Z", UI::QT::Helper::formatNumber( position.z, 'f', 3 ) );
			}
			break;
		}
		}
	}

	void TopologyEditorDialog::_updateBondInspector( const Index p_index )
	{
		const auto & topology	   = App::REG().get<Core::Struct::Topology>( _system );
		const Index	 firstAtom	   = topology.getBondFirstAtom( p_index );
		const Index	 secondAtom	   = topology.getBondSecondAtom( p_index );
		const Index	 firstResidue  = topology.getAtomResidueIndex( firstAtom );
		const Index	 secondResidue = topology.getAtomResidueIndex( secondAtom );
		const Index	 firstChain	   = topology.getResidueChainIndex( firstResidue );
		const Index	 secondChain   = topology.getResidueChainIndex( secondResidue );

		_clearSelectionInspector();
		_selectedType->setText( "Bond" );
		_selectedName->setText( QString( "%1 - %2" )
									.arg( _textOrDash( topology.getAtomName( firstAtom ) ) )
									.arg( _textOrDash( topology.getAtomName( secondAtom ) ) ) );
		_setInspectorProperty(
			0, "Order", QString::fromStdString( std::string( _bondOrder( topology.getBondOrder( p_index ) ) ) )
		);
		_setInspectorProperty(
			1,
			"First Atom",
			QString( "%1 (%2)" )
				.arg( _textOrDash( topology.getAtomName( firstAtom ) ) )
				.arg( UI::QT::Helper::formatNumber( firstAtom ) )
		);
		_setInspectorProperty(
			2,
			"Second Atom",
			QString( "%1 (%2)" )
				.arg( _textOrDash( topology.getAtomName( secondAtom ) ) )
				.arg( UI::QT::Helper::formatNumber( secondAtom ) )
		);
		_setInspectorProperty( 3, "Residue A", _textOrDash( topology.getResidueName( firstResidue ) ) );
		_setInspectorProperty( 4, "Residue B", _textOrDash( topology.getResidueName( secondResidue ) ) );
		_setInspectorProperty(
			5,
			firstChain == secondChain ? "Chain" : "Chains",
			firstChain == secondChain ? _textOrDash( topology.getChainName( firstChain ) )
									  : QString( "%1 / %2" )
											.arg( _textOrDash( topology.getChainName( firstChain ) ) )
											.arg( _textOrDash( topology.getChainName( secondChain ) ) )
		);
	}
} // namespace VTX::Tool::TopologyEditor::Dialog
