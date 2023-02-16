#ifndef __VTX_UI_WIDGET_RENDER_OVERLAY_CAMERA_QUICK_ACCESS__
#define __VTX_UI_WIDGET_RENDER_OVERLAY_CAMERA_QUICK_ACCESS__

#include "base_overlay.hpp"
#include <QToolButton>
#include <QWidget>

namespace VTX::UI::Widget::Render::Overlay
{
	class CameraQuickAccess : public BaseOverlay
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~CameraQuickAccess() = default;
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		CameraQuickAccess( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshCameraProjectionButton();

	  private:
		QToolButton * _cameraProjectionButton = nullptr;

		void _toggleCameraProjection();
	};
} // namespace VTX::UI::Widget::Render::Overlay
#endif
