#ifndef __VTX_UI_WIDGET_RENDER__
#define __VTX_UI_WIDGET_RENDER__

#include "opengl_widget.hpp"
#include "overlay/base_overlay.hpp"
#include "overlay/visualization_quick_access.hpp"
#include "ui/old_ui/event/base_event_firerer_input.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include "ui/old_ui/ui/widget/render/base_integrated_widget.hpp"
#include <QFocusEvent>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <app/core/model/base_model.hpp>
#include <app/core/view/base_view.hpp>
#include <app/internal/worker/snapshoter.hpp>
#include <app/mvc.hpp>
#include <map>

namespace VTX::UI::Widget::Render
{
	class RenderWidget : public BaseManualWidget<QWidget>, public VTX::UI::Event::BaseEventFirererInput
	{
		VTX_WIDGET

		friend Worker::Snapshoter;

	  public:
		~RenderWidget();
		void localize() override;

		inline bool isOpenGLValid() const { return _openGLWidget->isValid(); }
		void		updateRender() const;
		void		updateRenderSetting( const App::Render::Renderer::RENDER_SETTING );
		const Vec2i getPickedIds( const uint p_x, const uint p_y );

		void displayOverlay( const Overlay::OVERLAY & p_overlay, const Overlay::OVERLAY_ANCHOR & p_position );
		Overlay::BaseOverlay *		 getOverlay( const Overlay::OVERLAY & p_overlay );
		const Overlay::BaseOverlay * getOverlay( const Overlay::OVERLAY & p_overlay ) const;

		void setOverlayVisibility( const Overlay::OVERLAY & p_overlay, const bool p_visible );
		void showAllOverlays( const bool p_show );

		void			 receiveEvent( const VTX::App::Core::Event::VTXEvent & p_event ) override;
		const ID::VTX_ID getEventFirererId() const override { return ID::UI::Input::RENDER_WIDGET; }

	  protected:
		RenderWidget( QWidget * p_parent );

		void resizeEvent( QResizeEvent * p_event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void mouseMoveEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void mousePressEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void mouseReleaseEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void mouseDoubleClickEvent( QMouseEvent * const p_event ) override { _fireEventInput( p_event ); }
		void keyPressEvent( QKeyEvent * const p_event ) override { _fireEventInput( p_event ); }
		void keyReleaseEvent( QKeyEvent * const p_event ) override { _fireEventInput( p_event ); }
		void wheelEvent( QWheelEvent * const p_event ) override { _fireEventInput( p_event ); }

	  private:
		OpenGLWidget *									   _openGLWidget = nullptr;
		std::map<Overlay::OVERLAY, Overlay::BaseOverlay *> _overlays
			= std::map<Overlay::OVERLAY, Overlay::BaseOverlay *>();
		std::vector<BaseIntegratedWidget *> _integratedWidgets = std::vector<BaseIntegratedWidget *>();

		inline OpenGLWidget & getOpenGLWidget()
		{
			assert( _openGLWidget != nullptr );
			return *_openGLWidget;
		}

		void _addIntegratedWidget( BaseIntegratedWidget * const p_widget );

		Overlay::BaseOverlay * _instantiateOverlay( const Overlay::OVERLAY & p_overlayType );

		template<typename V,
				 typename M,
				 typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<App::Core::View::BaseView<M>, V>::value>,
				 typename = std::enable_if<std::is_base_of<BaseIntegratedWidget, V>::value>>
		void _removeViewIntegratedWidget( const M * const p_model, const ID::VTX_ID & p_viewName )
		{
			V * const integratedWidgetView = VTX::MVC_MANAGER().getView<V>( p_model, p_viewName );

			_integratedWidgets.erase(
				std::find( _integratedWidgets.begin(), _integratedWidgets.end(), integratedWidgetView ) );

			VTX::MVC_MANAGER().deleteView( p_model, p_viewName );
		}

		// Shortcuts.
		void _onShortcutToggleCameraController();
		void _onShortcutResetCameraController();
		void _onShortcutToggleCamera();
		void _onShortcutAddViewpoint();
		void _onShortcutSnapshot();
		void _onShortcutChangeRenderMode();
		void _onShortcutPrintCameraInfos();
		void _onShortcutToggleAllOverlays();
	};
} // namespace VTX::UI::Widget::Render
#endif
