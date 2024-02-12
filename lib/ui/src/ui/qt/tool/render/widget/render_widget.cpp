#include "ui/qt/tool/render/widget/render_widget.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QShortcut>
#include <app/vtx_app.hpp>
#include <renderer/renderer.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Tool::Render::Widget
{
	RenderWidget::RenderWidget( QWidget * p_parent ) : QtPanelTemplate( p_parent )
	{
		name			  = "Render";
		defaultSize		  = Style::RENDER_PREFERRED_SIZE;
		visibleByDefault  = true;
		referenceInPanels = false;
	}

	RenderWidget::~RenderWidget() {}

	void RenderWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_openGLWidget = WidgetFactory::get().instantiateWidget<Widget::OpenGLWidget>( this, "openglWidget" );

		setFocusPolicy( Qt::StrongFocus );

		QVBoxLayout * const layout = new QVBoxLayout( this );
		layout->setContentsMargins( 1, 1, 1, 1 );

		_openGLWidget->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );
		layout->addWidget( _openGLWidget );

		setMouseTracking( true );
		_openGLWidget->setMouseTracking( true );
	}

	void RenderWidget::_setupSlots() {}

	void RenderWidget::makeCurrentContext() { getOpenGLWidget().makeCurrent(); }
	void RenderWidget::doneCurrentContext() { getOpenGLWidget().doneCurrent(); }
} // namespace VTX::UI::QT::Tool::Render::Widget
