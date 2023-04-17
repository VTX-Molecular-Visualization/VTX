#ifndef __VTX_UI_QT_TOOL_UI_FEATURES_INFORMATION_TOOL__
#define __VTX_UI_QT_TOOL_UI_FEATURES_INFORMATION_TOOL__

#include "ui/core/tool_registration.hpp"
#include "ui/qt/base_qt_tool.hpp"
#include "ui/qt/tool/keys.hpp"
#include "widget/information_widget.hpp"

namespace VTX::UI::QT::Tool::UIFeatures
{
	class InformationTool : public BaseQtTool
	{
		REGISTER_TOOL( InformationTool, UI_FEATURE_INFORMATION );

	  public:
		InformationTool();
		void instantiateTool() override;

	  private:
		UIFeatures::Widget::InformationWidget * _informationWidget = nullptr;

		void _addButtonsInMainMenu();

		void _displayInformationWindow();
		void onInformationWindowFinished( const int p_result );
	};
} // namespace VTX::UI::QT::Tool::UIFeatures

#endif
