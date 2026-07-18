#include "ui/qt/dock_widget/options.hpp"
#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/model/system_model.hpp"
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
#include <app/action/accessibility.hpp>
#include <app/action/action_manager.hpp>
#include <app/action/application.hpp>
#include <app/network/network_manager.hpp>
#include <app/services.hpp>
#include <app/session.hpp>
#include <app/setting/accessibility.hpp>
#include <string>
#include <ui/qt/widget/sequence.hpp>
#include <util/enum.hpp>
#include <util/event_hub.hpp>
#include <util/string.hpp>
#include <util/type_traits.hpp>
#include <util/types.hpp>

namespace
{
	const QString _TEXT_DISK_USAGE = "%1 : %2";

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

		// Tree.
		auto * groupBoxTree = new QGroupBox( "System tree" );
		auto * layoutTree	= new QVBoxLayout( groupBoxTree );

		_comboBoxTreeViewMode = new QComboBox( this );
		_comboBoxTreeViewMode->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		_comboBoxTreeViewMode->setMinimumWidth( 0 );

		_comboBoxTreeViewMode->addItem( "By chain", toUnderlying( Model::SystemModel::ViewMode::ByChain ) );
		_comboBoxTreeViewMode->addItem( "By category", toUnderlying( Model::SystemModel::ViewMode::ByCategory ) );

		const int treeViewMode
			= SETTINGS()
				  .value( SETTING_KEY_TREE_VIEW_MODE, toUnderlying( Model::SystemModel::ViewMode::ByChain ) )
				  .toInt();
		const int treeViewModeIndex = _comboBoxTreeViewMode->findData( treeViewMode );
		if ( treeViewModeIndex != -1 )
		{
			_comboBoxTreeViewMode->setCurrentIndex( treeViewModeIndex );
		}

		connect(
			_comboBoxTreeViewMode,
			&QComboBox::currentIndexChanged,
			[ this ]( const int )
			{
				const int viewMode = _comboBoxTreeViewMode->currentData().toInt();
				SETTINGS().setValue( SETTING_KEY_TREE_VIEW_MODE, viewMode );
				App::HUB().trigger<Events::TreeViewModeChanged>( viewMode );
			}
		);

		layoutTree->addWidget( _comboBoxTreeViewMode );

		// Sequence.
		auto * groupBoxSeq = new QGroupBox( "Sequence viewer" );
		auto * layoutSeq   = new QVBoxLayout( groupBoxSeq );

		_comboBoxSeqViewMode = new QComboBox( this );
		_comboBoxSeqViewMode->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		_comboBoxSeqViewMode->setMinimumWidth( 0 );

		_comboBoxSeqViewMode->addItem( "Author Residue IDs", toUnderlying( Model::SystemModel::ViewMode::ByChain ) );
		_comboBoxSeqViewMode->addItem( "Contiguous IDs", toUnderlying( Model::SystemModel::ViewMode::ByCategory ) );

		const int seqViewMode
			= SETTINGS()
				  .value( SETTING_KEY_SEQUENCE_VTX_RESID, toUnderlying( Widget::Sequence::Mode::OriginalResId ) )
				  .toInt();
		const int seqViewModeIndex = _comboBoxSeqViewMode->findData( seqViewMode );
		if ( seqViewModeIndex != -1 )
		{
			_comboBoxSeqViewMode->setCurrentIndex( seqViewModeIndex );
		}

		connect(
			_comboBoxSeqViewMode,
			&QComboBox::currentIndexChanged,
			[ this ]( const int )
			{
				const int viewMode = _comboBoxSeqViewMode->currentData().toInt();
				SETTINGS().setValue( SETTING_KEY_SEQUENCE_VTX_RESID, viewMode );
				App::HUB().trigger<Events::SequenceResIdChanged>( viewMode );
			}
		);

		layoutSeq->addWidget( _comboBoxSeqViewMode );

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
		const auto &   accessibility	 = App::ECS::getFirstComponent<App::Setting::Accessibility>();
		const bool	   forceDyslexicFont = accessibility.forceDyslexicFont;
		_comboBoxFont->setCurrentText(
			forceDyslexicFont ? SETTINGS().value( SETTING_KEY_FONT, Style::DEFAULT_FONT_FAMILY ).toString()
							  : STYLE().getCurrentFontFamily()
		);

		connect(
			_comboBoxFont,
			&QComboBox::currentTextChanged,
			[ this ]( const QString & p_fontName )
			{
				SETTINGS().setValue( SETTING_KEY_FONT, p_fontName );
				if ( not _checkBoxForceDyslexicFont->isChecked() )
				{
					STYLE().setFontFamily( p_fontName );
				}
			}
		);

		layoutDisplay->addWidget( _comboBoxFont );

		// Reset layout.
		auto * buttonResetLayout
			= new ActionablePushButton( UI_ACTIONS().getAction( Action::Theme::RESET_LAYOUT ), this );
		buttonResetLayout->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		buttonResetLayout->setMinimumWidth( 0 );
		layoutDisplay->addWidget( buttonResetLayout );

		// Accessibility.
		auto * groupBoxAccessibility = new QGroupBox( "Accessibility" );
		auto * layoutAccessibility	 = new QVBoxLayout( groupBoxAccessibility );

		layoutAccessibility->addWidget( new QLabel( "Color", this ) );

		_comboBoxColorAccessibilityMode = new QComboBox( this );
		_comboBoxColorAccessibilityMode->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		_comboBoxColorAccessibilityMode->setMinimumWidth( 0 );
		_comboBoxColorAccessibilityMode->addItem(
			"Default", toUnderlying( App::Setting::E_COLOR_ACCESSIBILITY_MODE::DEFAULT )
		);
		_comboBoxColorAccessibilityMode->addItem(
			"High contrast", toUnderlying( App::Setting::E_COLOR_ACCESSIBILITY_MODE::HIGH_CONTRAST )
		);
		_comboBoxColorAccessibilityMode->addItem(
			"Colorblind-friendly", toUnderlying( App::Setting::E_COLOR_ACCESSIBILITY_MODE::COLORBLIND )
		);

		const int colorModeIndex = _comboBoxColorAccessibilityMode->findData( toUnderlying( accessibility.colorMode ) );
		if ( colorModeIndex != -1 )
		{
			_comboBoxColorAccessibilityMode->setCurrentIndex( colorModeIndex );
		}

		connect(
			_comboBoxColorAccessibilityMode,
			&QComboBox::currentIndexChanged,
			[ this ]( const int )
			{
				const auto colorMode = static_cast<App::Setting::E_COLOR_ACCESSIBILITY_MODE>(
					_comboBoxColorAccessibilityMode->currentData().toUInt()
				);
				App::ACTION().execute<App::Action::Accessibility::SetColorMode>( colorMode );
			}
		);

		_checkBoxForceDyslexicFont = new QCheckBox( "Dyslexia-friendly font", this );
		_checkBoxForceDyslexicFont->setToolTip( "Use OpenDyslexic as the application font" );
		_checkBoxForceDyslexicFont->setWhatsThis( _checkBoxForceDyslexicFont->toolTip() );
		_checkBoxForceDyslexicFont->setChecked( forceDyslexicFont );
		_comboBoxFont->setEnabled( not forceDyslexicFont );

		connect(
			_checkBoxForceDyslexicFont,
			&QCheckBox::checkStateChanged,
			[ this ]( const int p_state )
			{
				const bool forceDyslexicFont = p_state == Qt::Checked;
				App::ACTION().execute<App::Action::Accessibility::SetForceDyslexicFont>( forceDyslexicFont );
				_comboBoxFont->setEnabled( not forceDyslexicFont );
				STYLE().setFontFamily( forceDyslexicFont ? Style::DYSLEXIC_FONT_FAMILY : _comboBoxFont->currentText() );
			}
		);

		layoutAccessibility->addWidget( _comboBoxColorAccessibilityMode );
		layoutAccessibility->addWidget( _checkBoxForceDyslexicFont );

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

		auto * groupBoxDiskUsage = _createDiskUsageGroupBox();

		_layout->addWidget( groupBoxTree );
		_layout->addWidget( groupBoxSeq );
		_layout->addWidget( groupBoxDisplay );
		_layout->addWidget( groupBoxAccessibility );
		_layout->addWidget( groupBoxInputs );
		_layout->addWidget( groupBoxGraphics );
		_layout->addWidget( groupBoxDiskUsage );
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
		SETTINGS().setValue( SETTING_KEY_TREE_VIEW_MODE, _comboBoxTreeViewMode->currentData().toInt() );
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

	QGroupBox * Options::_createDiskUsageGroupBox()
	{
		auto * groupBox = new QGroupBox( "Disk usage" );
		auto * layout	= new QVBoxLayout( groupBox );

		_addDiskUsageRow( layout, "Data", App::SESSION().getCacheDir(), _labelCacheSize );
		_addDiskUsageRow( layout, "HDRI", App::SESSION().getHdriDir(), _labelHdriSize );
		_addDiskUsageRow( layout, "Snapshots", App::SESSION().getSnapshotsDir(), _labelSnapshotsSize );
		_addDiskUsageRow( layout, "Logs", App::SESSION().getLogsDir(), _labelLogsSize );

		return groupBox;
	}

	void Options::_addDiskUsageRow(
		QVBoxLayout *	   p_layout,
		const QString &	   p_title,
		const FilePath &   p_path,
		QPointer<QLabel> & p_labelSize
	)
	{
		auto * layoutRow  = new QHBoxLayout();
		auto * buttonOpen = new QPushButton( this );

		buttonOpen->setIcon( STYLE().iconFromCodepoint( Style::Icons::OPEN ) );
		buttonOpen->setToolTip( QString( "Open %1 folder" ).arg( p_title.toLower() ) );
		buttonOpen->setWhatsThis( buttonOpen->toolTip() );
		buttonOpen->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Preferred );
		buttonOpen->setMinimumWidth( 0 );

		connect(
			buttonOpen,
			&QPushButton::clicked,
			[ p_path ]() { QDesktopServices::openUrl( QUrl::fromLocalFile( QDir( p_path ).absolutePath() ) ); }
		);

		p_labelSize = new QLabel( this );
		p_labelSize->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Preferred );
		p_labelSize->setMinimumWidth( 0 );

		layoutRow->addWidget( p_labelSize );
		layoutRow->addWidget( buttonOpen );
		p_layout->addLayout( layoutRow );
	}

	void Options::_refreshFolderInfos(
		const FilePath & p_path,
		QLabel * const	 p_labelSize,
		const QString &	 p_title
	) const
	{
		const QDir			dir( p_path );
		const QFileInfoList list = dir.entryInfoList();
		size_t				size = 0;
		for ( const auto & info : list )
		{
			size += info.size();
		}

		p_labelSize->setText(
			_TEXT_DISK_USAGE.arg( p_title, QString::fromStdString( Util::String::memSizeToStr( size ) ) )
		);
	}

	void Options::_refreshFoldersInfos()
	{
		_refreshFolderInfos( App::SESSION().getCacheDir(), _labelCacheSize, "Data" );
		_refreshFolderInfos( App::SESSION().getHdriDir(), _labelHdriSize, "HDRI" );
		_refreshFolderInfos( App::SESSION().getSnapshotsDir(), _labelSnapshotsSize, "Snapshots" );
		_refreshFolderInfos( App::SESSION().getLogsDir(), _labelLogsSize, "Logs" );
	}

} // namespace VTX::UI::QT::DockWidget
