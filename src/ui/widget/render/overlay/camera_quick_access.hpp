#ifndef __VTX_UI_WIDGET_RENDER_OVERLAY_CAMERA_QUICK_ACCESS__
#define __VTX_UI_WIDGET_RENDER_OVERLAY_CAMERA_QUICK_ACCESS__

#include "base_overlay.hpp"
#include "model/renderer/render_effect_preset.hpp"
#include "ui/object/background_dependent_icon.hpp"
#include "view/callback_view.hpp"
#include <QToolButton>
#include <QWidget>

namespace VTX::UI::Widget::Render::Overlay
{
	class CameraQuickAccess : public BaseOverlay
	{
		Q_OBJECT
		VTX_WIDGET

	  private:
		using RenderEffectView = View::CallbackView<Model::Renderer::RenderEffectPreset, CameraQuickAccess>;

	  public:
		~CameraQuickAccess();
		void localize() override;

		void receiveEvent( const Event::VTXEvent & p_event ) override;

	  protected:
		CameraQuickAccess( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshCameraProjectionButton();

	  private:
		QToolButton *						_cameraProjectionButton = nullptr;
		UI::Object::BackgroundDependentIcon _projectionPerspectiveIcon;
		UI::Object::BackgroundDependentIcon _projectionOrthographicIcon;

		void _onRenderEffectChange( const Event::VTXEvent * const p_event );
		void _refreshCameraProjectionIconColor() const;
		void _attachViewOnAppliedRenderEffect();

		void _toggleCameraProjection();
	};
} // namespace VTX::UI::Widget::Render::Overlay
#endif
