#include "ui/qt/widget/renderer/panel.hpp"
#include "ui/qt/style.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QShortcut>
#include <app/vtx_app.hpp>
#include <renderer/facade.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget::Renderer
{
	Panel::Panel( QWidget * p_parent ) : QtPanelTemplate( p_parent )
	{
		name			  = "Render";
		defaultSize		  = Style::RENDER_PREFERRED_SIZE;
		visibleByDefault  = true;
		referenceInPanels = false;
	}

	Panel::~Panel() {}

	void Panel::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_openGLWidget = WidgetFactory::get().instantiateWidget<OpenGL>( this, "openglWidget" );

		setFocusPolicy( Qt::StrongFocus );

		QVBoxLayout * const layout = new QVBoxLayout( this );
		layout->setContentsMargins( 1, 1, 1, 1 );

		_openGLWidget->setSizePolicy( QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred );
		layout->addWidget( _openGLWidget );

		setMouseTracking( true );
		_openGLWidget->setMouseTracking( true );
	}

	void Panel::_setupSlots() {}

	void Panel::makeCurrentContext() { getOpenGLWidget().makeCurrent(); }
	void Panel::doneCurrentContext() { getOpenGLWidget().doneCurrent(); }
} // namespace VTX::UI::QT::Widget::Renderer
