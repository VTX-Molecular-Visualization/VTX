#ifndef __VTX_UI_TOOL_DEFAULT_SESSION_LOADER__
#define __VTX_UI_TOOL_DEFAULT_SESSION_LOADER__

#include "__new_archi/ui/core/tool_registration.hpp"
#include "__new_archi/ui/qt/widget/main_menu/menu_toolbutton_widget.hpp"
#include "__new_archi/ui/qt/widget/scene/scene_item_widget_instancier.hpp"
#include "__new_archi/ui/qt/widget/scene/scene_widget.hpp"
#include "keys.hpp"
#include "object3d/scene.hpp"
#include <QMenu>
#include <QObject>

namespace VTX::UI::DefaultTools
{
	class SessionLoader : public VTX::UI::Core::BaseVTXUITool, public QObject
	{
		REGISTER_TOOL( SessionLoader, MOLECULE_LOADER_KEY );

	  public:
		SessionLoader();
		void instantiateTool() override;

	  private:
		QMenu *										 _recentSessionMenu		  = nullptr;
		QT::Widget::MainMenu::MenuToolButtonWidget * _openRecentSessionButton = nullptr;

		void _addButtonsInMainMenu();
		void _addActionsInContextualMenus();

	  private:
		void _newSession() const;
		void _downloadMoleculeFile() const;
		void _openFile() const;
		void _loadRecentSession( const int & p_ptrSessionIndex ) const;
		void _saveSession() const;
		void _saveAsSession() const;

		void _refreshRecentFiles();
	};
} // namespace VTX::UI::DefaultTools

#endif
