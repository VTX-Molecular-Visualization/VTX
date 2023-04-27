#include "ui/old_ui/ui/widget/dialog/set_trajectory_target_dialog.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QPushButton>

#include <app/action/main.hpp>
#include <app/component/chemistry/molecule.hpp>
#include <app/old_app/object3d/scene.hpp>
#include <app/application/setting.hpp>
#include <string>

namespace VTX::UI::Widget::Dialog
{
	MoleculeListWidget::ModelFieldLine::ModelFieldLine( CustomWidget::BaseModelListWidget * const p_parent ) :
		CustomWidget::BaseModelFieldLine( p_parent )
	{
		_checkBox	= new QCheckBox( p_parent );
		_modelLabel = new QLabel( p_parent );
	}
	MoleculeListWidget::ModelFieldLine ::~ModelFieldLine()
	{
		if ( _checkBox != nullptr )
			delete _checkBox;
		_checkBox = nullptr;

		if ( _modelLabel != nullptr )
			delete _modelLabel;
		_modelLabel = nullptr;
	}

	App::Core::Model::BaseModel * const MoleculeListWidget::ModelFieldLine::getModel() const { return _linkedMolecule; };
	void					 MoleculeListWidget::ModelFieldLine::setModel( App::Core::Model::BaseModel * const p_model )
	{
		_linkedMolecule = static_cast<App::Component::Chemistry::Molecule *>( p_model );

		if ( _linkedMolecule == nullptr )
			_modelLabel->setText( "-" );
		else
			_modelLabel->setText( QString::fromStdString( _linkedMolecule->getDisplayName() ) );
	}

	bool MoleculeListWidget::ModelFieldLine::isTicked() const { return _checkBox->isChecked(); }
	void MoleculeListWidget::ModelFieldLine::setTickState( const bool p_ticked ) const
	{
		_checkBox->setChecked( p_ticked );
	}

	MoleculeListWidget::MoleculeListWidget( QWidget * p_parent ) : CustomWidget::BaseModelListWidget( p_parent ) {}

	void MoleculeListWidget::_initColumns()
	{
		_initColumn( int( GRID_LAYOUT_COLUMN::TICK ), "Target", 0 );
		_initColumn( int( GRID_LAYOUT_COLUMN::MODEL ), "Molecule", 1000 );
	}

	CustomWidget::BaseModelFieldLine * const MoleculeListWidget::_instantiateLine()
	{
		ModelFieldLine * const newLine = new ModelFieldLine( this );
		return newLine;
	}
	void MoleculeListWidget::_addLineInLayout( CustomWidget::BaseModelFieldLine * const p_line, const int p_row )
	{
		const ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( p_line );

		_addWidgetInColumn( castedLine->getCheckBox(), p_row, int( GRID_LAYOUT_COLUMN::TICK ) );
		_addWidgetInColumn( castedLine->getModelLabel(), p_row, int( GRID_LAYOUT_COLUMN::MODEL ) );
	}

	std::vector<App::Component::Chemistry::Molecule *> MoleculeListWidget::getTickedMolecules() const
	{
		std::vector<App::Component::Chemistry::Molecule *> res = std::vector<App::Component::Chemistry::Molecule *>();
		res.reserve( _getLines().size() );

		for ( CustomWidget::BaseModelFieldLine * const line : _getLines() )
		{
			ModelFieldLine * const castedLine = dynamic_cast<ModelFieldLine * const>( line );
			if ( castedLine->isTicked() )
				res.emplace_back( castedLine->getMolecule() );
		}

		res.shrink_to_fit();

		return res;
	}

	void MoleculeListWidget::tickMolecule( App::Component::Chemistry::Molecule * const p_molecule, const bool p_ticked )
	{
		_findLineFromModel<ModelFieldLine>( p_molecule )->setTickState( p_ticked );
	}

	/////////////////////////////////////////////////////////////////////////////////////////

	SetTrajectoryTargetDialog & SetTrajectoryTargetDialog::_getInstance()
	{
		if ( _instance == nullptr )
		{
			_instance = WidgetFactory::get().instantiateWidget<Dialog::SetTrajectoryTargetDialog>(
				&VTXApp::get().getMainWindow(), "setTrajectoryTargetDialog" );
		}

		return *_instance;
	}
	SetTrajectoryTargetDialog::SetTrajectoryTargetDialog( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void SetTrajectoryTargetDialog::openDialog() { _getInstance().show(); }
	void SetTrajectoryTargetDialog::openDialog( const FilePath & p_trajectoryPath )
	{
		_getInstance()._filepath = p_trajectoryPath;
		_getInstance().show();
	}

	void SetTrajectoryTargetDialog::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		this->setWindowFlag( Qt::WindowFlags::enum_type::FramelessWindowHint, true );
		this->setWindowModality( Qt::WindowModality::ApplicationModal );

		QVBoxLayout * const verticalLayout = new QVBoxLayout( this );

		_linkToMoleculeRadioButton = new QRadioButton( this );
		_linkToMoleculeRadioButton->setText( "Select target to trajectory" );
		_linkToMoleculeRadioButton->setChecked( true );

		QHBoxLayout * const moleculeListWidgetLayout = new QHBoxLayout( nullptr );

		_moleculeListWidget = WidgetFactory::get().instantiateWidget<MoleculeListWidget>( this, "moleculeListWidget" );
		moleculeListWidgetLayout->addSpacing( Style::DATA_GRID_HORIZONTAL_SPACE * 2 );
		moleculeListWidgetLayout->addWidget( _moleculeListWidget );

		_createNewMoleculeRadioButton = new QRadioButton( this );
		_createNewMoleculeRadioButton->setText( "Create a new molecule" );

		_dialogButtons
			= new QDialogButtonBox( QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open,
									Qt::Orientation::Horizontal,
									this );

		_trajectoryImportModeButtonGroup = new QButtonGroup( this );
		_trajectoryImportModeButtonGroup->addButton( _linkToMoleculeRadioButton );
		_trajectoryImportModeButtonGroup->addButton( _createNewMoleculeRadioButton );

		verticalLayout->addWidget( _linkToMoleculeRadioButton );
		verticalLayout->addLayout( moleculeListWidgetLayout );
		verticalLayout->addWidget( _createNewMoleculeRadioButton );
		verticalLayout->addWidget( _dialogButtons );

		_refresh();
	}

	void SetTrajectoryTargetDialog::_setupSlots()
	{
		connect( _trajectoryImportModeButtonGroup,
				 &QButtonGroup::buttonToggled,
				 this,
				 &SetTrajectoryTargetDialog::_onTrajectoryImportModeChange );

		QPushButton * cancelButton = _dialogButtons->button( QDialogButtonBox::StandardButton::Cancel );
		connect( cancelButton, &QPushButton::clicked, this, &SetTrajectoryTargetDialog::cancelAction );

		QPushButton * openButton = _dialogButtons->button( QDialogButtonBox::StandardButton::Open );
		connect( openButton, &QPushButton::clicked, this, &SetTrajectoryTargetDialog::openAction );
	}
	void SetTrajectoryTargetDialog::localize() { this->setWindowTitle( "Download Molecule" ); }

	void SetTrajectoryTargetDialog::showEvent( QShowEvent * p_event )
	{
		BaseManualWidget::showEvent( p_event );
		_refresh();
	}

	void SetTrajectoryTargetDialog::cancelAction() { close(); }
	void SetTrajectoryTargetDialog::openAction()
	{
		if ( _linkToMoleculeRadioButton->isChecked() )
		{
			const std::vector<App::Component::Chemistry::Molecule *> selectedMolecules = _moleculeListWidget->getTickedMolecules();
			VTX_ACTION( new App::Action::Main::Open( _filepath, selectedMolecules ) );
		}
		else if ( _createNewMoleculeRadioButton->isChecked() )
		{
			VTX_ACTION( new App::Action::Main::Open( _filepath ) );
		}

		close();
	}

	void SetTrajectoryTargetDialog::_refresh()
	{
		_moleculeListWidget->clearModels();

		const std::string filename = _filepath.stem().string();

		std::vector<App::Component::Chemistry::Molecule *> molecules = std::vector<App::Component::Chemistry::Molecule *>();
		molecules.reserve( VTXApp::get().getScene().getMolecules().size() );

		for ( const Object3D::Scene::PairMoleculePtrFloat & pairMolecule : VTXApp::get().getScene().getMolecules() )
			molecules.emplace_back( pairMolecule.first );

		VTXApp::get().getScene().sortMoleculesBySceneIndex( molecules );

		for ( App::Component::Chemistry::Molecule * const molecule : molecules )
		{
			_moleculeListWidget->addModel( molecule );
			_moleculeListWidget->tickMolecule( molecule, molecule->getDisplayName() == filename );
		}
	}

	void SetTrajectoryTargetDialog::_onTrajectoryImportModeChange( QAbstractButton * p_button, const bool p_checked )
	{
		if ( p_button == _linkToMoleculeRadioButton )
		{
			_moleculeListWidget->setEnabled( p_checked );
		}
	}

} // namespace VTX::UI::Widget::Dialog
