#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/actionable_push_button.hpp"
#include "ui/qt/widget/renderer.hpp"
#include <QApplication>
#include <QDesktopServices>
#include <QFileDialog>
#include <QGroupBox>
#include <QPushButton>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/network/network_manager.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <string>
#include <util/enum.hpp>
#include <util/event_hub.hpp>
#include <util/string.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>

namespace
{
	const QString _TEXT_CACHE_COUNT = "Files : %1";
	const QString _TEXT_CACHE_SIZE	= "Size : %1";

	VTX::UI::QT::ActionRegistry::ActionParams _themeParams( const VTX::UI::QT::Style::E_THEME p_theme )
	{
		return VTX::UI::QT::ActionRegistry::ActionParams { { std::string( VTX::UI::QT::Action::Theme::PARAM_THEME ),
															 static_cast<int>( VTX::toUnderlying( p_theme ) ) } };
	}

} // namespace

namespace VTX::UI::QT::DockWidget
{

	Options::Options( QWidget * p_parent ) : BaseDockWidget( p_parent, "Options" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::OPTIONS ) );

		using namespace Widget;

		// Display.
		// Theme.
		auto * groupBoxDisplay = new QGroupBox( "Display" );
		auto * layoutDisplay   = new QVBoxLayout( groupBoxDisplay );

		_comboBoxTheme = new QComboBox( this );
		_comboBoxTheme->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		_comboBoxTheme->setMinimumWidth( 0 );

		_comboBoxTheme->addItem( "System", static_cast<int>( toUnderlying( Style::E_THEME::SYSTEM ) ) );
		_comboBoxTheme->addItem( "Light", static_cast<int>( toUnderlying( Style::E_THEME::LIGHT ) ) );
		_comboBoxTheme->addItem( "Dark", static_cast<int>( toUnderlying( Style::E_THEME::DARK ) ) );
		_syncThemeComboBox();

		connect(
			_comboBoxTheme,
			&QComboBox::currentIndexChanged,
			[ this ]( const int )
			{
				const auto theme = static_cast<Style::E_THEME>( _comboBoxTheme->currentData().toInt() );
				UI_ACTIONS().trigger( Action::Theme::SET, _themeParams( theme ) );
			}
		);
		App::HUB().connect<Events::ThemeChanged, &Options::_onThemeChanged>( this );

		layoutDisplay->addWidget( _comboBoxTheme );

		// Font.
		_comboBoxFont = new QComboBox( this );
		_comboBoxFont->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		_comboBoxFont->setMinimumWidth( 0 );

		uint i = 0;
		for ( const QString & fontName : STYLE().getAvailableFonts() )
		{
			_comboBoxFont->addItem( fontName );
			_comboBoxFont->setItemData( i++, QFont( fontName, Style::DEFAULT_FONT_SIZE ), Qt::FontRole );
		}

		QSignalBlocker blocker( _comboBoxFont );
		_comboBoxFont->setCurrentText( STYLE().getCurrentFontFamily() );

		connect(
			_comboBoxFont,
			&QComboBox::currentTextChanged,
			[ this ]( const QString & p_fontName ) { STYLE().setFontFamily( p_fontName ); }
		);

		layoutDisplay->addWidget( _comboBoxFont );

		// Reset layout.
		auto * buttonResetLayout
			= new ActionablePushButton( UI_ACTIONS().getAction( Action::Theme::RESET_LAYOUT ), this );
		buttonResetLayout->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		buttonResetLayout->setMinimumWidth( 0 );
		layoutDisplay->addWidget( buttonResetLayout );

		// Inputs.
		auto * groupBoxInputs = new QGroupBox( "Inputs" );
		auto * layoutInputs	  = new QVBoxLayout( groupBoxInputs );
		_comboBoxKBLayout	  = new QComboBox( this );
		_comboBoxKBLayout->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		_comboBoxKBLayout->setMinimumWidth( 0 );

		const Widget::KB_LAYOUT defaultLayout
			= QLocale::system().language() == QLocale::French ? Widget::KB_LAYOUT::AZERTY : Widget::KB_LAYOUT::QWERTY;
		const Widget::KB_LAYOUT layout = static_cast<Widget::KB_LAYOUT>(
			SETTINGS().value( SETTING_KEY_KEYBOARD_LAYOUT, toUnderlying( defaultLayout ) ).toUInt()
		);

		for ( int l = 0; l < toUnderlying( Widget::KB_LAYOUT::COUNT ); l++ )
		{
			const Widget::KB_LAYOUT kbLayout   = static_cast<Widget::KB_LAYOUT>( l );
			QString					layoutName = QString::fromStdString( Util::Enum::enumName( kbLayout ).data() );
			_comboBoxKBLayout->addItem( layoutName, QVariant::fromValue( kbLayout ) );
		}

		connect(
			_comboBoxKBLayout,
			&QComboBox::currentIndexChanged,
			[ this ]( int p_layout ) { App::HUB().trigger<Events::KeyboardLayoutChanged>( p_layout ); }
		);

		_comboBoxKBLayout->setCurrentIndex( toUnderlying( layout ) );

		layoutInputs->addWidget( _comboBoxKBLayout );

		// Graphics.
		auto * groupBoxGraphics = new QGroupBox( "Graphics" );
		auto * layoutGraphics	= new QVBoxLayout( groupBoxGraphics );
		_checkBoxVSync			= new QCheckBox( "Vertical synchronization", this );
		_checkBoxVSync->setToolTip( "Synchronize the frame rate with the screen refresh rate" );
		_checkBoxVSync->setWhatsThis( "Synchronize the frame rate with the screen refresh rate" );

		connect(
			_checkBoxVSync,
			&QCheckBox::checkStateChanged,
			[ this ]( const int p_state )
			{ App::ACTION().execute<App::Action::Application::SetVSync>( p_state == Qt::Checked ); }
		);

		layoutGraphics->addWidget( _checkBoxVSync );

		_checkBoxSavePower = new QCheckBox( "Save GPU power", this );
		_checkBoxSavePower->setToolTip( "Render frame only on update" );
		_checkBoxSavePower->setWhatsThis( "Render frame only on update" );

		connect(
			_checkBoxSavePower,
			&QCheckBox::checkStateChanged,
			[ this ]( const int p_state )
			{ App::ACTION().execute<App::Action::Application::SetSavePower>( p_state == Qt::Checked ); }
		);

		layoutGraphics->addWidget( _checkBoxSavePower );

		auto * groupBoxCache
			= _createFolderGroupBox( "Data cache", App::SESSION().getCacheDir(), _labelCacheCount, _labelCacheSize );
		auto * groupBoxSnapshots = _createFolderGroupBox(
			"Snapshots", App::SESSION().getSnapshotsDir(), _labelSnapshotsCount, _labelSnapshotsSize
		);
		auto * groupBoxLogs
			= _createFolderGroupBox( "Logs", App::SESSION().getLogsDir(), _labelLogsCount, _labelLogsSize );

		_layout->addWidget( groupBoxDisplay );
		_layout->addWidget( groupBoxInputs );
		_layout->addWidget( groupBoxGraphics );
		_layout->addWidget( groupBoxCache );
		_layout->addWidget( groupBoxSnapshots );
		_layout->addWidget( groupBoxLogs );
		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		_refreshFoldersInfos();

		// Settings.
		QSignalBlocker b( _checkBoxVSync ); // Do not emit, opengl widget already knows.
		_checkBoxVSync->setChecked( SETTINGS().value( SETTING_KEY_VSYNC, Renderer::VSYNC_DEFAULT ).toBool() );
		_checkBoxSavePower->setChecked( SETTINGS().value( SETTING_KEY_SAVE_POWER, SAVE_POWER_DEFAULT ).toBool() );

		connect(
			this,
			&QDockWidget::visibilityChanged,
			[ this ]( const bool p_visible )
			{
				if ( p_visible )
				{
					_refreshFoldersInfos();
				}
			}
		);

		connect(
			qApp,
			&QApplication::focusChanged,
			this,
			[ this ]( QWidget *, QWidget * const p_now )
			{
				if ( isVisible() && p_now != nullptr && ( p_now == this || isAncestorOf( p_now ) ) )
				{
					_refreshFoldersInfos();
				}
			}
		);
	}

	Options::~Options()
	{
		SETTINGS().setValue( SETTING_KEY_KEYBOARD_LAYOUT, _comboBoxKBLayout->currentIndex() );
		SETTINGS().setValue( SETTING_KEY_VSYNC, _checkBoxVSync->isChecked() );
		SETTINGS().setValue( SETTING_KEY_SAVE_POWER, _checkBoxSavePower->isChecked() );
	}

	void Options::_syncThemeComboBox()
	{
		const int theme = toUnderlying( STYLE().getCurrentTheme() );
		const int index = _comboBoxTheme->findData( theme );
		if ( index != -1 )
		{
			QSignalBlocker blocker( _comboBoxTheme );
			_comboBoxTheme->setCurrentIndex( index );
		}
	}

	void Options::_onThemeChanged( const Events::ThemeChanged & ) { _syncThemeComboBox(); }

	QGroupBox * Options::_createFolderGroupBox(
		const QString &	   p_title,
		const FilePath &   p_path,
		QPointer<QLabel> & p_labelCount,
		QPointer<QLabel> & p_labelSize
	)
	{
		auto * groupBox = new QGroupBox( p_title );
		auto * layout	= new QVBoxLayout( groupBox );

		auto * layoutButton = new QHBoxLayout();
		auto * buttonOpen	= new QPushButton( "Open", this );
		auto * buttonClear	= new QPushButton( "Clear", this );

		buttonOpen->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		buttonClear->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		buttonOpen->setMinimumWidth( 0 );
		buttonClear->setMinimumWidth( 0 );

		connect(
			buttonOpen,
			&QPushButton::clicked,
			[ p_path ]() { QDesktopServices::openUrl( QUrl::fromLocalFile( QDir( p_path ).absolutePath() ) ); }
		);

		connect(
			buttonClear,
			&QPushButton::clicked,
			[ this, p_path ]()
			{
				QDir( p_path ).removeRecursively();
				_refreshFoldersInfos();
			}
		);

		p_labelCount = new QLabel( this );
		p_labelSize	 = new QLabel( this );

		layout->addWidget( p_labelCount );
		layout->addWidget( p_labelSize );
		layoutButton->addWidget( buttonOpen );
		layoutButton->addWidget( buttonClear );
		layoutButton->setStretch( 0, 1 );
		layoutButton->setStretch( 1, 1 );
		layoutButton->setStretch( 2, 1 );
		layout->addLayout( layoutButton );

		return groupBox;
	}

	void Options::_refreshFolderInfos(
		const FilePath & p_path,
		QLabel * const	 p_labelCount,
		QLabel * const	 p_labelSize
	) const
	{
		const QDir			dir( p_path );
		const uint			fileCount = dir.entryList( QDir::Files ).size();
		const QFileInfoList list	  = dir.entryInfoList();
		size_t				size	  = 0;
		for ( const auto & info : list )
		{
			size += info.size();
		}

		p_labelCount->setText( _TEXT_CACHE_COUNT.arg( fileCount ) );
		p_labelSize->setText( _TEXT_CACHE_SIZE.arg( QString::fromStdString( Util::String::memSizeToStr( size ) ) ) );
	}

	void Options::_refreshFoldersInfos()
	{
		_refreshFolderInfos( App::SESSION().getCacheDir(), _labelCacheCount, _labelCacheSize );
		_refreshFolderInfos( App::SESSION().getSnapshotsDir(), _labelSnapshotsCount, _labelSnapshotsSize );
		_refreshFolderInfos( App::SESSION().getLogsDir(), _labelLogsCount, _labelLogsSize );
	}

} // namespace VTX::UI::QT::DockWidget
