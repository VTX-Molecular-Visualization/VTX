#include "download_molecule_dialog.hpp"
#include "action/main.hpp"
#include "ui/main_window.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"
#include <QPushButton>
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

	void DownloadMoleculeDialog::openDialog() { _getInstance().show(); }
	void DownloadMoleculeDialog::openDialog( const QString & p_txt )
	{
		_getInstance()._fileLineEdit->setText( p_txt );
		openDialog();
	}

	DownloadMoleculeDialog::DownloadMoleculeDialog( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

	void DownloadMoleculeDialog::cancelAction() { close(); }
	void DownloadMoleculeDialog::openAction()
	{
		const std::string lineEditText = _fileLineEdit->text().toStdString();
		VTX_ACTION( new Action::Main::OpenApi( lineEditText ) );

		close();
	}

	void DownloadMoleculeDialog::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		this->setWindowFlag( Qt::WindowFlags::enum_type::FramelessWindowHint, true );
		this->setWindowModality( Qt::WindowModality::ApplicationModal );

		QVBoxLayout * verticalLayout = new QVBoxLayout( this );

		_fileLineEdit = new QLineEdit( this );
		verticalLayout->addWidget( _fileLineEdit );

		_dialogButtons
			= new QDialogButtonBox( QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open,
									Qt::Orientation::Horizontal,
									this );
		verticalLayout->addWidget( _dialogButtons );

		_fileLineEdit->setFocus();
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
		_fileLineEdit->setPlaceholderText( "Enter pdb id code" );
	}
} // namespace VTX::UI::Widget::Dialog
