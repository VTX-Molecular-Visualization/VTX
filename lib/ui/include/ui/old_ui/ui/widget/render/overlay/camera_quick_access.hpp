#ifndef __VTX_UI_WIDGET_RENDER_OVERLAY_CAMERA_QUICK_ACCESS__
#define __VTX_UI_WIDGET_RENDER_OVERLAY_CAMERA_QUICK_ACCESS__

#include "base_overlay.hpp"
#include "ui/old_ui/ui/object/background_dependent_icon.hpp"
#include <QAction>
#include <QMenu>
#include <QToolButton>
#include <QWidget>
#include <app/old_app/model/renderer/render_effect_preset.hpp>
#include <app/view/callback_view.hpp>

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

		void receiveEvent( const VTX::Event::VTXEvent & p_event ) override;

	  protected:
		CameraQuickAccess( QWidget * p_parent );
		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshCameraProjectionButton();

	  private:
		QToolButton * _cameraProjectionButton  = nullptr;
		QMenu *		  _renderEffectLibraryMenu = nullptr;
		QToolButton * _exportImageButton	   = nullptr;

		UI::Object::BackgroundDependentIcon _projectionPerspectiveIcon;
		UI::Object::BackgroundDependentIcon _projectionOrthographicIcon;
		UI::Object::BackgroundDependentIcon _exportImageIcon;

		void _refreshIconColors() const;
		void _refreshRenderEffectMenu() const;

		void _attachViewOnAppliedRenderEffect();

		void _onRenderEffectChange( const VTX::Event::VTXEvent * const p_event );
		void _toggleCameraProjection();
		void _applyRenderEffectPresetAction( const QAction * const p_action );
		void _onExportImageClickedAction() const;
	};
} // namespace VTX::UI::Widget::Render::Overlay
#endif
