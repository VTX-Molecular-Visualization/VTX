#include "ui/qt/dialog/download.hpp"
#include "ui/qt/helper.hpp"
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <app/action/action_manager.hpp>
#include <app/action/io.hpp>
#include <app/action/scene.hpp>
#include <app/constants.hpp>
#include <app/session.hpp>
#include <memory>

namespace
{
	constexpr QUtf8StringView PDB_ID_TEMPLATE { VTX::App::SYSTEM_REPLACEMENT_TOKEN };
	constexpr QUtf8StringView DEFAULT_URL { VTX::App::RCSB_PDB_DOWNLOAD_URL_TEMPLATE };
	constexpr uint			  MAX_HISTORY_SIZE = 10;
} // namespace

namespace VTX::UI::QT::Dialog
{
	Download::Download()
	{
		setWindowTitle( "Download" );
		// TODO: try size policy?
		setFixedSize( 300, 180 );

		auto * layout = Helper::makeQObject<QVBoxLayout>( this );

		// URL.
		auto * labelURL = Helper::makeQObject<QLabel>( "Server URL", this );
		_comboBoxURL	= Helper::makeQObject<QComboBox>( this );
		_comboBoxURL->setEditable( true );
		_comboBoxURL->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		// PDB id.
		auto * labelPDB = Helper::makeQObject<QLabel>( "PDB id", this );
		_comboBoxPDB	= Helper::makeQObject<QComboBox>( this );
		_comboBoxPDB->setEditable( true );
		_comboBoxPDB->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );
		_comboBoxPDB->setFocus();

		// Buttons.
		auto * buttonBox = Helper::makeQObject<QDialogButtonBox>(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open, this );

		// Cache found label.
		// QLabel * labelCache = new QLabel( "File found", this );

		layout->addWidget( labelURL );
		layout->addWidget( _comboBoxURL );
		layout->addWidget( labelPDB );
		layout->addWidget( _comboBoxPDB );

		// Radio buttons.
		auto * buttonGroup		= Helper::makeQObject<QButtonGroup>( this );
		auto * layoutCacheRadio = Helper::makeChildLayout<QHBoxLayout>( *layout );
		_radioButtonOpen		= Helper::makeQObject<QRadioButton>( "Open from cache", this );
		_radioButtonDownload	= Helper::makeQObject<QRadioButton>( "Download", this );
		buttonGroup->addButton( _radioButtonOpen );
		buttonGroup->addButton( _radioButtonDownload );
		layoutCacheRadio->addWidget( _radioButtonOpen );
		layoutCacheRadio->addWidget( _radioButtonDownload );

		// Layout.
		layout->addWidget( buttonBox );

		// Callbacks.
		// Update radio button if file is found in cache.
		connect( _comboBoxPDB,
				 &QComboBox::editTextChanged,
				 [ this ]( const QString & )
				 {
					 auto pdb = _comboBoxPDB->currentText().toStdString();
					 pdb	  = Util::String::trim( pdb );
					 if ( pdb.length() == 4 )
					 {
						 const FilePath path = App::SESSION().getCacheDir()
											   / ( pdb + std::string( VTX::App::RCSB_PDB_DOWNLOAD_FILE_EXTENSION ) );
						 if ( std::filesystem::exists( path ) )
						 {
							 // Show radio buttons.
							 _radioButtonOpen->setEnabled( true );
							 _radioButtonDownload->setEnabled( true );
							 _radioButtonOpen->setChecked( true );
							 return;
						 }
					 }
					 // Hide radio buttons.
					 _radioButtonOpen->setEnabled( false );
					 _radioButtonDownload->setEnabled( true );
					 _radioButtonDownload->setChecked( true );
				 } );

		// Open button.
		connect( buttonBox,
				 &QDialogButtonBox::accepted,
				 [ this ]()
				 {
					 // Check inputs.
					 _url = _comboBoxURL->currentText().trimmed();
					 _pdb = _comboBoxPDB->currentText().trimmed();

					 FilePath path
						 = App::SESSION().getCacheDir()
						   / ( _pdb.toStdString() + std::string( VTX::App::RCSB_PDB_DOWNLOAD_FILE_EXTENSION ) );
					 if ( std::filesystem::exists( path ) and _radioButtonOpen->isChecked() )
					 {
						 App::ACTION().execute<App::Action::IO::LoadSystem>( path );
					 }
					 else
					 {
						 if ( _url.isEmpty() )
						 {
							 VTX_ERROR( "URL is empty" );
							 return;
						 }

						 if ( _pdb.isEmpty() )
						 {
							 VTX_ERROR( "PDB id is empty" );
							 return;
						 }

						 if ( _pdb.length() != 4 )
						 {
							 VTX_ERROR( "PDB id must be 4 characters" );
							 return;
						 }
						 const std::string urlTemplate = _url.toStdString();
						 if ( urlTemplate.find( App::SYSTEM_REPLACEMENT_TOKEN ) != std::string::npos )
						 {
							 App::ACTION().execute<App::Action::IO::DownloadSystem>(
								 Util::String::replaceFirst(
									 urlTemplate, std::string( App::SYSTEM_REPLACEMENT_TOKEN ), _pdb.toStdString() ),
								 _pdb.toStdString() + std::string( VTX::App::RCSB_PDB_DOWNLOAD_FILE_EXTENSION ) );
						 }
						 else
						 {
							 // TODO: Use fmt formater for QString.
							 VTX_ERROR( "URL does not contain {}", PDB_ID_TEMPLATE.toString().toStdString() );
							 return;
						 }
					 }

					 _saveHistory( SETTING_KEY_URL, _url );
					 _saveHistory( SETTING_KEY_PDB, _pdb );

					 accept();
				 } );

		connect( buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept );
		connect( buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject );

		// Load history.
		_loadHistory( SETTING_KEY_URL, _comboBoxURL );
		_loadHistory( SETTING_KEY_PDB, _comboBoxPDB );

		// FIXME: Avoid losing default url if not in history.
		const QString defaultUrl = DEFAULT_URL.toString();
		if ( _comboBoxURL->findText( defaultUrl ) == -1 )
		{
			_comboBoxURL->addItem( defaultUrl );
		}
	}

	void Download::_saveHistory( const QUtf8StringView p_key, const QString & p_value )
	{
		QStringList history = SETTINGS().value( p_key ).toStringList();
		// Remove duplicates.
		history.removeAll( p_value );
		// Prepend the last one.
		history.prepend( p_value );
		// Limit to 10.
		while ( history.size() > MAX_HISTORY_SIZE )
		{
			history.removeLast();
		}
		SETTINGS().setValue( p_key, history );
	}

	void Download::_loadHistory( const QUtf8StringView p_key, QComboBox * const p_comboBox )
	{
		const QStringList history = SETTINGS().value( p_key ).toStringList();
		p_comboBox->addItems( history );
	}
} // namespace VTX::UI::QT::Dialog
