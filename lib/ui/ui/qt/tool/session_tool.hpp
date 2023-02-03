#ifndef __VTX_UI_QT_TOOL_SESSION__
#define __VTX_UI_QT_TOOL_SESSION__

#include "core/tool_registration.hpp"
#include "qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "keys.hpp"
#include "src/object3d/scene.hpp"
#include <QMenu>
#include <QObject>

namespace VTX::UI::QT::Tool
{
	class SessionTool : public VTX::UI::Core::BaseVTXUITool, public QObject
	{
		REGISTER_TOOL( SessionTool, SESSION_TOOLS_KEY );

	  public:
		SessionTool();
		void instantiateTool() override;

	  private:
		QMenu *										 _recentSessionMenu		  = nullptr;
		QT::Widget::MainMenu::MenuToolButtonWidget * _openRecentSessionButton = nullptr;

		void _addButtonsInMainMenu();
		void _addActionsInContextualMenus();
		void _registerShortcuts();

	  private:
		void _newSession() const;
		void _downloadMoleculeFile() const;
		void _openFile() const;
		void _loadRecentSession( const int & p_ptrSessionIndex ) const;
		void _saveSession() const;
		void _saveAsSession() const;

		void _refreshRecentFiles();
	};
} // namespace VTX::UI::QT::Tool

#endif
