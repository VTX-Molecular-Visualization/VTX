#include "setting_widget.hpp"
#include "old_ui/style.hpp"
#include "old_ui/ui/widget_factory.hpp"
#include "old_ui/vtx_app.hpp"
#include "setting_color_widget.hpp"
#include "setting_menu_item.hpp"
#include "setting_molecule_widget.hpp"
#include "setting_render_effect_widget.hpp"
#include "setting_representation_widget.hpp"
#include "setting_vtx_widget.hpp"

namespace VTX::UI::Widget::Settings
{
	SettingWidget::SettingWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent ) {}

	SettingWidget::~SettingWidget()
	{
		for ( const SettingMenuItem * const menu : _menus )
		{
			delete menu;
		}
	}

	void SettingWidget::receiveEvent( const VTX::Event::VTXEvent & p_event ) {}

	void SettingWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_menu = new CustomWidget::DockWindowMainWidget<QWidget>(
			Style::SETTINGS_PREFERRED_SIZE, Style::SETTINGS_MINIMUM_SIZE, this );
		_menu->setObjectName( "settingMenuBar" );
		_menu->setSizePolicy( QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding );

		QHBoxLayout * const horizontalLayout = new QHBoxLayout( _menu );
		horizontalLayout->setSizeConstraint( QLayout::SetMinAndMaxSize );
		QWidget * const menuLayoutWidget = new QWidget( _menu );
		menuLayoutWidget->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Expanding );
		_menuLayout = new QVBoxLayout( menuLayoutWidget );
		_menuLayout->setContentsMargins( 0, 0, 0, 0 );
		_menuLayout->setSizeConstraint( QLayout::SetFixedSize );

		QWidget * const contentLayoutWidget = new QWidget( _menu );
		contentLayoutWidget->setContentsMargins( 0, 0, 0, 0 );
		_contentLayout = new QVBoxLayout( contentLayoutWidget );

		_title = new QLabel( this );
		_contentLayout->addWidget( _title, 0 );

		_addItem( WidgetFactory::get().instantiateWidget<SettingRepresentationWidget>( _menu, "settingMenuButton" ),
				  SETTING_MENU::REPRESENTATIONS,
				  "Representations" );
		_addItem( WidgetFactory::get().instantiateWidget<SettingRenderEffectWidget>( _menu, "settingMenuButton" ),
				  SETTING_MENU::RENDER_EFFECTS,
				  "Render Effects" );
		// !V0.1
		//_addItem( WidgetFactory::get().instantiateWidget<SettingColorWidget>( _menu, "settingMenuButton" ),
		//		  SETTING_MENU::COLORS,
		//		  "Colors" );

		_addItem( WidgetFactory::get().instantiateWidget<SettingMoleculeWidget>( _menu, "settingMenuButton" ),
				  SETTING_MENU::MOLECULE,
				  "Molecule" );
		_addItem( WidgetFactory::get().instantiateWidget<SettingVTXWidget>( _menu, "settingMenuButton" ),
				  SETTING_MENU::VTX,
				  "VTX" );

		setCurrentMenu( _currentMenu, true );

		horizontalLayout->addWidget( menuLayoutWidget );
		horizontalLayout->addWidget( contentLayoutWidget );

		horizontalLayout->setAlignment( menuLayoutWidget, Qt::AlignmentFlag::AlignTop );

		setWidget( _menu );
	}

	void SettingWidget::_setupSlots() {};
	void SettingWidget::localize()
	{
		// Qt translate (not use currently)
		setWindowTitle( "Settings" );
	};

	void SettingWidget::_addItem( QWidget * const p_itemContent, const SETTING_MENU & p_menu, const QString & p_title )
	{
		SettingMenuItem * const menuItem = new SettingMenuItem( this, p_menu, p_itemContent, p_title );

		_menus.emplace_back( menuItem );

		_menuLayout->addWidget( menuItem->getButton() );
		_contentLayout->addWidget( menuItem->getContent(), 100 );
	}

	void SettingWidget::setCurrentMenu( const SETTING_MENU & p_menuIndex, const bool p_force )
	{
		if ( p_force || _currentMenu != p_menuIndex )
		{
			_menus[ int( _currentMenu ) ]->setVisible( false );
			_currentMenu = p_menuIndex;
			_menus[ int( _currentMenu ) ]->setVisible( true );
			_title->setText( _menus[ int( _currentMenu ) ]->getTitle() );
		}
	}

} // namespace VTX::UI::Widget::Settings
