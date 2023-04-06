#include "information_tool.hpp"
#include "qt/application_qt.hpp"
#include "qt/main_window.hpp"
#include "qt/tool/keys.hpp"
#include "qt/widget/main_menu/menu_toolblock_widget.hpp"
#include "qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "qt/widget/main_menu/menu_tooltab_widget.hpp"
#include "qt/widget_factory.hpp"
#include <QWidget>

namespace VTX::UI::QT::Tool::UIFeatures
{
	InformationTool::InformationTool() : BaseQtTool() {}

	void InformationTool::instantiateTool() { _addButtonsInMainMenu(); }

	void InformationTool::_addButtonsInMainMenu()
	{
		QT::MainWindow * const						mainWindow = &QT::QT_APP()->getMainWindow();
		QT::Widget::MainMenu::MenuToolBlockWidget & toolBlock  = mainWindow->getMainMenuToolBlock( getLayoutData() );

		QT::Widget::MainMenu::MenuToolButtonWidget * const informationButton
			= WidgetFactory::get().instantiateWidget<QT::Widget::MainMenu::MenuToolButtonWidget>( &toolBlock,
																								  "informationButton" );
		informationButton->setData( "About", ":/sprite/info_button.png", Qt::Orientation::Vertical );
		informationButton->setTriggerAction( this, &InformationTool::_displayInformationWindow );

		toolBlock.pushButton( *informationButton );
	}

	void InformationTool::_displayInformationWindow()
	{
		if ( _informationWidget == nullptr )
		{
			QT::MainWindow * const mainWindow = &QT::QT_APP()->getMainWindow();

			_informationWidget = WidgetFactory::get().instantiateWidget<UIFeatures::Widget::InformationWidget>(
				mainWindow, "informationWidget" );

			connect( _informationWidget,
					 &UIFeatures::Widget::InformationWidget::finished,
					 this,
					 &InformationTool::onInformationWindowFinished );
		}

		_informationWidget->show();
		_informationWidget->raise();
	}

	void InformationTool::onInformationWindowFinished( const int p_result )
	{
		if ( _informationWidget != nullptr )
		{
			delete ( _informationWidget );
			_informationWidget = nullptr;
		}
	}

} // namespace VTX::UI::QT::Tool::UIFeatures
