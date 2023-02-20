#ifndef __VTX_UI_WIDGET_MENU_CAMERA__
#define __VTX_UI_WIDGET_MENU_CAMERA__

#include "background_block.hpp"
#include "camera_action_block.hpp"
#include "render_effects_block.hpp"
#include "snapshot_block.hpp"
#include "ui/widget/main_menu/menu_tooltab_widget.hpp"
#include "viewpoint_block.hpp"
#include "windows_block.hpp"
#include <QWidget>

namespace VTX::UI::Widget::MainMenu::Camera
{
	class Toolbar : public MenuTooltabWidget
	{
		VTX_WIDGET

	  public:
		~Toolbar();
		void localize() override;

	  protected:
		Toolbar( QWidget * p_parent = nullptr );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		CameraActionBlock *	 _cameraAction	= nullptr;
		BackgroundBlock *	 _background	= nullptr;
		RenderEffectsBlock * _renderEffects = nullptr;
		ViewpointBlock *	 _viewpoints	= nullptr;
		SnapshotBlock *		 _snapshot		= nullptr;
		WindowsBlock *		 _windows		= nullptr;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
