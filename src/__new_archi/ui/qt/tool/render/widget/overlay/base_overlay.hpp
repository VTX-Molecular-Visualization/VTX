#ifndef __VTX_UI_QT_TOOL_RENDER_WIDGET_BASE_OVERLAY__
#define __VTX_UI_QT_TOOL_RENDER_WIDGET_BASE_OVERLAY__

#include "__new_archi/ui/qt/widget/base_manual_widget.hpp"
#include <QAction>
#include <QEvent>
#include <QMenu>
#include <QObject>
#include <QToolBar>
#include <QWidget>

namespace VTX::UI::QT::Tool::Render::Widget::Overlay
{
	enum class OVERLAY : int
	{
		VISUALIZATION_QUICK_ACCESS,
	};

	enum class OVERLAY_ANCHOR : int
	{
		BOTTOM_CENTER,
		NONE,
	};

	class BaseOverlay : public QT::Widget::BaseManualWidget<QToolBar>
	{
		Q_OBJECT
		NEW_ARCHI_VTX_WIDGET

	  public:
		~BaseOverlay() = default;
		void localize() override;

		void setAnchorPosition( const OVERLAY_ANCHOR & p_anchor );
		void updatePosition( const QSize & p_renderRect );

		QAction * addMenu( QMenu * const p_menu );

	  protected:
		BaseOverlay( QWidget * p_parent );

		bool eventFilter( QObject * p_obj, QEvent * p_event ) override;

		void _setupUi( const QString & p_name ) override;
		void _setupSlots() override;

		void _refreshSize();

	  private:
		OVERLAY_ANCHOR _anchorPosition = OVERLAY_ANCHOR::NONE;

		QPoint						 _getPosition( const QSize & p_rect );
		std::map<QMenu *, QAction *> _mapMenuActions = std::map<QMenu *, QAction *>();
	};
} // namespace VTX::UI::QT::Tool::Render::Widget::Overlay
#endif
