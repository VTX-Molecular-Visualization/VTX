#ifndef __VTX_UI_WIDGET_MENU_CAMERA_SNAPSHOT_BLOCK__
#define __VTX_UI_WIDGET_MENU_CAMERA_SNAPSHOT_BLOCK__

#include <app/event/vtx_event.hpp>
#include "ui/old_ui/ui/widget/main_menu/menu_toolblock_widget.hpp"
#include "ui/old_ui/ui/widget/main_menu/menu_toolbutton_widget.hpp"
#include <app/old_app/view/base_view.hpp>
#include <QWidget>
#include <vector>

namespace VTX::UI
{
	enum class WindowMode;
}

namespace VTX::UI::Widget::MainMenu::Camera
{
	class SnapshotBlock : public MenuToolBlockWidget
	{
		VTX_WIDGET
		VTX_VIEW

	  public:
		void localize() override;

	  protected:
		SnapshotBlock( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

	  private:
		// Render Feature
		MenuToolButtonWidget * _takeSnapshot  = nullptr;
		MenuToolButtonWidget * _exportAsImage = nullptr;

		// ACTIONS //
		void _takeSnapshotAction() const;
		void _exportAsImageAction() const;
	};
} // namespace VTX::UI::Widget::MainMenu::Camera
#endif
