#ifndef __VTX_UI_WIDGET_RENDER_BASE_OVERLAY__
#define __VTX_UI_WIDGET_RENDER_BASE_OVERLAY__

#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget/base_manual_widget.hpp"
#include <QAction>
#include <QEvent>
#include <QMenu>
#include <QObject>
#include <QToolBar>
#include <QWidget>
#include <string>
#include <vector>

namespace VTX::UI::Widget::Render::Overlay
{
	enum class OVERLAY : int
	{
		VISUALIZATION_QUICK_ACCESS,
		CAMERA_PROJECTION_QUICK_ACCESS,

		COUNT,
	};
	inline static const std::vector<std::string> OVERLAY_NAMES_STR = { "Controller", "Rendering" };

	enum class OVERLAY_ANCHOR : int
	{
		BOTTOM_CENTER,
		TOP_RIGHT,
		NONE,
	};

	class BaseOverlay : public BaseManualWidget<QToolBar>
	{
		Q_OBJECT
		VTX_WIDGET

	  public:
		~BaseOverlay() = default;
		void localize() override;

		void setStyle( const Style::RENDER_OVERLAY_STYLE & p_style );
		void setAnchorPosition( const OVERLAY_ANCHOR & p_anchor );
		void updatePosition( const QSize & p_renderRect );

		QAction * addMenu( QMenu * const p_menu );

	  protected:
		BaseOverlay( QWidget * p_parent );

		bool eventFilter( QObject * p_obj, QEvent * p_event ) override;
		void mousePressEvent( QMouseEvent * event ) override;
		void mouseReleaseEvent( QMouseEvent * event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshSize();

	  private:
		OVERLAY_ANCHOR _anchorPosition = OVERLAY_ANCHOR::NONE;

		QPoint						 _getPosition( const QSize & p_rect );
		std::map<QMenu *, QAction *> _mapMenuActions = std::map<QMenu *, QAction *>();
	};
} // namespace VTX::UI::Widget::Render::Overlay
#endif
