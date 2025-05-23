#include "ui/qt/dialog/download.hpp"
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <app/action/io.hpp>
#include <app/action/scene.hpp>
#include <app/core/network/network_system.hpp>

namespace VTX::UI::QT::Dialog
{

	Download::Download()
	{
		setWindowTitle( "Download" );
		// TODO: try size policy?
		setFixedSize( 300, 180 );

		auto * layout = new QVBoxLayout( this );

		// URL.
		auto * labelURL = new QLabel( "Server URL", this );
		_comboBoxURL	= new QComboBox( this );
		_comboBoxURL->setEditable( true );
		_comboBoxURL->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );

		// PDB id.
		auto * labelPDB = new QLabel( "PDB id", this );
		_comboBoxPDB	= new QComboBox( this );
		_comboBoxPDB->setEditable( true );
		_comboBoxPDB->setInsertPolicy( QComboBox::InsertPolicy::NoInsert );
		_comboBoxPDB->setFocus();

		// Buttons.
		QDialogButtonBox * buttonBox = new QDialogButtonBox(
			QDialogButtonBox::StandardButton::Cancel | QDialogButtonBox::StandardButton::Open, this
		);

		// Cache found label.
		// QLabel * labelCache = new QLabel( "File found", this );

		// Radio buttons.
		auto * buttonGroup		= new QButtonGroup( this );
		auto * layoutCacheRadio = new QHBoxLayout( this );
		_radioButtonOpen		= new QRadioButton( "Open from cache" );
		_radioButtonDownload	= new QRadioButton( "Download" );
		buttonGroup->addButton( _radioButtonOpen );
		buttonGroup->addButton( _radioButtonDownload );
		layoutCacheRadio->addWidget( _radioButtonOpen );
		layoutCacheRadio->addWidget( _radioButtonDownload );

		// Layout.
		layout->addWidget( labelURL );
		layout->addWidget( _comboBoxURL );
		layout->addWidget( labelPDB );
		layout->addWidget( _comboBoxPDB );
		layout->addLayout( layoutCacheRadio );
		layout->addWidget( buttonBox );

		// Callbacks.
		// Update radio button if file is found in cache.
		connect(
			_comboBoxPDB,
			&QComboBox::editTextChanged,
			[ this ]( const QString & )
			{
				auto pdb = _comboBoxPDB->currentText().toStdString();
				Util::String::trim( pdb );
				if ( pdb.length() == 4 )
				{
					FilePath path = App::Filesystem::getCachePath( pdb + ".pdb" );
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
			}
		);

		// Open button.
		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Open ),
			&QPushButton::clicked,
			[ this ]()
			{
				// Check inputs.
				_url = _comboBoxURL->currentText().trimmed();
				_pdb = _comboBoxPDB->currentText().trimmed();

				FilePath path = App::Filesystem::getCachePath( _pdb.toStdString() + ".pdb" );
				if ( std::filesystem::exists( path ) and _radioButtonOpen->isChecked() )
				{
					App::ACTION_SYSTEM().execute<App::Action::Scene::LoadSystem>( path );
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
					Util::Url::UrlTemplate urlReplaced { _url.toStdString().data() };
					if ( not urlReplaced.hasReplacementToken() )
					{
						VTX_ERROR( "URL does not contain {}", _PDB_ID_TEMPLATE.toStdString() );
					}
					else
					{
						App::ACTION_SYSTEM().execute<App::Action::Io::DownloadSystem>(
							Util::Url::UrlFull( urlReplaced, Util::Url::SystemId( _pdb.toStdString().data() ) ),
							_pdb.toStdString() + ".pdb"
						);
					}
				}

				save();
				close();
			}
		);

		// Cancel button.
		connect(
			buttonBox->button( QDialogButtonBox::StandardButton::Cancel ),
			&QPushButton::clicked,
			[ this ]() { close(); }
		);

		// Load histories manually because dialog is destroyed when closed.
		restore();

		// FIXME: Avoid losing default url if not in history.
		if ( _comboBoxURL->findText( _DEFAULT_URL ) == -1 )
		{
			_comboBoxURL->addItem( _DEFAULT_URL );
		}
	}

	void Download::_loadHistory( const QString & p_key, QComboBox * const p_comboBox )
	{
		QStringList history = SETTINGS.value( p_key ).toStringList();
		p_comboBox->addItems( history );
	}

	void Download::_saveHistory( const QString & p_key, const QString & p_value )
	{
		QStringList history = SETTINGS.value( p_key ).toStringList();
		// Remove duplicates.
		if ( history.contains( p_value ) )
		{
			history.removeAll( p_value );
		}
		// Prepend the last one.
		history.prepend( p_value );
		// Limit to 10.
		while ( history.size() > _MAX_HISTORY_SIZE )
		{
			history.removeLast();
		}
		SETTINGS.setValue( p_key, history );
	}

	void Download::save()
	{
		_saveHistory( _SETTING_KEY_URL, _url );
		_saveHistory( _SETTING_KEY_PDB, _pdb );
	}

	void Download::restore()
	{
		_loadHistory( _SETTING_KEY_URL, _comboBoxURL );
		_loadHistory( _SETTING_KEY_PDB, _comboBoxPDB );
	}

} // namespace VTX::UI::QT::Dialog
