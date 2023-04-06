#ifndef __VTX_UI_QT_TOOL_RENDER_WIDGET_RENDER__
#define __VTX_UI_QT_TOOL_RENDER_WIDGET_RENDER__

#include "core/define.hpp"
#include "qt/qt_panel.hpp"
#include "qt/tool/render/widget/base_integrated_widget.hpp"
#include "qt/tool/render/widget/opengl_widget.hpp"
#include "qt/tool/render/widget/overlay/base_overlay.hpp"
#include "qt/widget/base_manual_widget.hpp"
#include <QFocusEvent>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <map>
#include <old/event/base_event_firerer_input.hpp>
#include <old/model/base_model.hpp>
#include <old/mvc/mvc_manager.hpp>
#include <old/view/base_view.hpp>
#include <old/worker/snapshoter.hpp>

namespace VTX::UI::QT::Tool::Render::Widget
{
	class RenderWidget : public QtPanelTemplate<QWidget>, public Event::BaseEventFirererInput
	{
		NEW_ARCHI_VTX_WIDGET

		friend VTX::Worker::Snapshoter;

	  public:
		~RenderWidget();

		PANEL_TYPE getPanelType() const override { return PANEL_TYPE::CENTRAL_WIDGET; };

		inline bool isOpenGLValid() const { return _openGLWidget->isValid(); }
		void		updateRender() const;
		void		updateRenderSetting( const Renderer::RENDER_SETTING );
		const Vec2i getPickedIds( const uint p_x, const uint p_y );

		void displayOverlay( const Overlay::OVERLAY & p_overlay, const Overlay::OVERLAY_ANCHOR & p_position );
		void hideOverlay( const Overlay::OVERLAY & p_overlay );

		void			 receiveEvent( const Event::VTXEvent & p_event ) override;
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
				 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>,
				 typename = std::enable_if<std::is_base_of<BaseIntegratedWidget, V>::value>>
		void _removeViewIntegratedWidget( const M * const p_model, const ID::VTX_ID & p_viewName )
		{
			V * const integratedWidgetView = MVC::MvcManager::get().getView<V>( p_model, p_viewName );

			_integratedWidgets.erase(
				std::find( _integratedWidgets.begin(), _integratedWidgets.end(), integratedWidgetView ) );

			MVC::MvcManager::get().deleteView( p_model, p_viewName );
		}

		// Shortcuts.
		void _onShortcutToggleCameraController();
		void _onShortcutResetCameraController();
		void _onShortcutAddViewpoint();
		void _onShortcutSnapshot();
		void _onShortcutChangeRenderMode();
		void _onShortcutPrintCameraInfos();
	};
} // namespace VTX::UI::QT::Tool::Render::Widget
#endif
