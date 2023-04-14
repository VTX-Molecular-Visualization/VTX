#include "download_molecule_dialog.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/widget_factory.hpp"
#include <QPushButton>
#include <QVBoxLayout>
#include <app/old_app/action/action_manager.hpp>
#include <app/old_app/action/main.hpp>
#include <app/old_app/setting.hpp>
#include <string>

namespace VTX::UI::QT::Tool::Session::Widget::Dialog
{
	DownloadMoleculeDialog * DownloadMoleculeDialog::pop()
	{
		DownloadMoleculeDialog * const dialog
			= QT::WidgetFactory::get().instantiateWidget<DownloadMoleculeDialog>( &QT_APP()->getMainWindow(), "" );

		dialog->_refreshComboBoxList();
		dialog->_fileComboBox->setFocus();

		dialog->show();

		return dialog;
	}
	DownloadMoleculeDialog * DownloadMoleculeDialog::pop( const QString & p_pdbId )
	{
		DownloadMoleculeDialog * const dialog = pop();
		dialog->setPdbId( p_pdbId );

		return dialog;
	}

	DownloadMoleculeDialog::DownloadMoleculeDialog( QWidget * p_parent ) : BaseDialog( p_parent ) {}

	void DownloadMoleculeDialog::setPdbId( const QString & p_id ) { _fileComboBox->setEditText( p_id ); }

	void DownloadMoleculeDialog::_setupUi( const QString & p_name )
	{
		BaseDialog::_setupUi( p_name );
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

	void DownloadMoleculeDialog::cancelAction() { close(); }
	void DownloadMoleculeDialog::openAction()
	{
		const std::string code = _fileComboBox->currentText().toStdString();
		VTX_ACTION( new Action::Main::OpenApi( code ) );

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

} // namespace VTX::UI::QT::Tool::Session::Widget::Dialog
