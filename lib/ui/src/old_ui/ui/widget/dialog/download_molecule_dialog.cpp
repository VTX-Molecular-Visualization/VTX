#include "ui/old_ui/ui/widget/dialog/download_molecule_dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QPushButton>

#include <app/action/main.hpp>
#include <app/application/setting.hpp>
#include <string>

namespace VTX::UI::Widget::Dialog
{
	DownloadMoleculeDialog & DownloadMoleculeDialog::_getInstance()
	{
		if ( _instance == nullptr )
		{
			_instance = WidgetFactory::get().instantiateWidget<Dialog::DownloadMoleculeDialog>(
				&VTXApp::get().getMainWindow(), "downloadMoleculeDialog" );
		}

		return *_instance;
	}
	DownloadMoleculeDialog::DownloadMoleculeDialog( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void DownloadMoleculeDialog::openDialog() { _getInstance().show(); }
	void DownloadMoleculeDialog::openDialog( const QString & p_txt )
	{
		_getInstance()._fileComboBox->setEditText( p_txt );
		openDialog();
	}

	void DownloadMoleculeDialog::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		this->setWindowFlag( Qt::WindowFlags::enum_type::FramelessWindowHint, true );
		this->setWindowModality( Qt::WindowModality::ApplicationModal );

		QVBoxLayout * const verticalLayout = new QVBoxLayout( this );

		_fileComboBox = new QComboBox( this );
		_fileComboBox->setEditable( true );
		_fileComboBox->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		verticalLayout->addWidget( _fileComboBox );

		_dialogButtons
			= new QDialogButtonBox( QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open,
									Qt::Orientation::Horizontal,
									this );
		verticalLayout->addWidget( _dialogButtons );

		_refreshComboBoxList();
		_fileComboBox->setFocus();
	}

	void DownloadMoleculeDialog::_setupSlots()
	{
		QPushButton * cancelButton = _dialogButtons->button( QDialogButtonBox::StandardButton::Cancel );
		connect( cancelButton, &QPushButton::clicked, this, &DownloadMoleculeDialog::cancelAction );

		QPushButton * openButton = _dialogButtons->button( QDialogButtonBox::StandardButton::Open );
		connect( openButton, &QPushButton::clicked, this, &DownloadMoleculeDialog::openAction );
	}
	void DownloadMoleculeDialog::localize()
	{
		this->setWindowTitle( "Download Molecule" );
		_fileComboBox->setPlaceholderText( "Enter pdb id code" );
	}

	void DownloadMoleculeDialog::showEvent( QShowEvent * p_event )
	{
		BaseManualWidget::showEvent( p_event );
		_refreshComboBoxList();
		_fileComboBox->setFocus();
	}

	void DownloadMoleculeDialog::cancelAction() { close(); }
	void DownloadMoleculeDialog::openAction()
	{
		const std::string code = _fileComboBox->currentText().toStdString();
		VTX_ACTION( new App::Action::Main::OpenApi( code ) );

		close();
	}

	void DownloadMoleculeDialog::_refreshComboBoxList()
	{
		_fileComboBox->clear();
		for ( const std::string & recentCodes : VTX_SETTING().recentDownloadCodes )
		{
			_fileComboBox->addItem( QString::fromStdString( recentCodes ) );
		}
		_fileComboBox->setCurrentIndex( 0 );
	}

} // namespace VTX::UI::Widget::Dialog
