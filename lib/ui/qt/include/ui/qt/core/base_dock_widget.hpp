#ifndef __VTX_UI_QT_CORE_BASE_DOCK_WIDGET__
#define __VTX_UI_QT_CORE_BASE_DOCK_WIDGET__

#include "ui/qt/core/base_widget.hpp"
#include <QBoxLayout>
#include <QDockWidget>
#include <QPointer>
#include <QScrollArea>
#include <concepts>

namespace VTX::UI::QT::Core
{
	template<typename L>
	concept ConceptLayout = std::is_base_of_v<QBoxLayout, L>;

	/**
	 * @brief Abstract class that describes a dock widget sizes, margins, and scroll area.
	 * @tparam DW is the derived class type.
	 */
	template<typename T, ConceptLayout L = QVBoxLayout>
	class BaseDockWidget : public Core::BaseWidget<T, QDockWidget>
	{
	  public:
		template<typename... Args>
		BaseDockWidget( Args &&... p_args ) : BaseWidget<T, QDockWidget>( std::forward<Args>( p_args )... )
		{
			// Force to set allowed areas in child classes.
			QDockWidget::setAllowedAreas( Qt::NoDockWidgetArea );

			// Scroll area.
			QDockWidget::setWidget( _scrollArea );
			_scrollArea->setWidgetResizable( true );
			_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
			_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
			_scrollArea->setFrameShape( QFrame::NoFrame );

			// Root widget and layout.
			_scrollArea->setWidget( _root );
			_root->setLayout( _layout );
			_layout = new L( _root );
			_layout->setContentsMargins( 0, 0, 0, 0 );
			//_layout->setSizeConstraint( QLayout::SetNoConstraint );
		}

		virtual ~BaseDockWidget() = default;

	  protected:
		QPointer<QScrollArea> _scrollArea = new QScrollArea();
		QPointer<QWidget>	  _root		  = new QWidget();
		QPointer<L>			  _layout;

	};
} // namespace VTX::UI::QT::Core

#endif
