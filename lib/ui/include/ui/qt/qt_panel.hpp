#ifndef __VTX_UI_QT_QT_PANEL__
#define __VTX_UI_QT_QT_PANEL__

#include "ui/core/base_panel.hpp"
#include "ui/qt/concepts.hpp"
#include "ui/qt/widget/base_manual_widget.hpp"
#include <QDialog>
#include <QDockWidget>
#include <QSize>
#include <QWidget>
#include <type_traits>

namespace VTX::UI::QT
{
	class QtPanel : public Core::BasePanel
	{
	  public:
		enum class PANEL_TYPE : int
		{
			DOCK_WIDGET,
			FLOATING_WINDOW,
			CENTRAL_WIDGET,
		};

	  public:
		QtPanel();
		~QtPanel();

		virtual PANEL_TYPE getPanelType() const = 0;

		void		 setVisible( const bool p_visible );
		virtual bool isVisible() const = 0;

		std::string name = "";
		QSize		defaultSize;
		bool		visibleByDefault  = false;
		bool		referenceInPanels = true;
		std::string section			  = "";

	  protected:
		virtual void _changeVisibility( const bool p_visible ) = 0;
	};

	template<QTWidgetConcept W>
	class QtPanelTemplate : public QtPanel, public Widget::BaseManualWidget<W>
	{
	  public:
		QtPanelTemplate( QWidget * const p_parent = nullptr ) : QtPanel(), Widget::BaseManualWidget<W>( p_parent ) {}
		virtual bool isVisible() const override { return W::isVisible(); };

		void localize() override { W::setWindowTitle( QString::fromStdString( name ) ); };

	  protected:
		virtual void _changeVisibility( const bool p_visible ) override { W::setVisible( p_visible ); };
	};

	class QtDockablePanel : public QtPanelTemplate<QDockWidget>
	{
	  public:
		QtDockablePanel( QWidget * const p_parent = nullptr );
		~QtDockablePanel();

		virtual PANEL_TYPE getPanelType() const override { return PANEL_TYPE::DOCK_WIDGET; };

		bool floatingByDefault = true;
		// For tabified
		Qt::DockWidgetArea defaultWidgetArea  = Qt::DockWidgetArea::NoDockWidgetArea;
		Qt::Orientation	   defaultOrientation = Qt::Orientation::Vertical;

	  protected:
		QWidget * _instantiateMainWidget( const QSize & p_preferredSize, const QSize & p_minimalSize );
	};

	class QtFloatingWindowPanel : public QtPanelTemplate<QDialog>
	{
	  public:
		QtFloatingWindowPanel( QWidget * const p_parent = nullptr );
		~QtFloatingWindowPanel();

		virtual PANEL_TYPE getPanelType() const override { return PANEL_TYPE::FLOATING_WINDOW; };
	};

} // namespace VTX::UI::QT

#endif
