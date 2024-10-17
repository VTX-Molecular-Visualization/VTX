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
	concept ConceptLayout = std::is_base_of_v<QLayout, L>;

	/**
	 * @brief Abstract class that describes dock widget sizes, margins, and scrollbars.
	 * @tparam T is the derived class type.
	 * @tparam HSA is the vertical scroll area flag.
	 * @tparam VSA is the horizontal scroll area flag.
	 * @tparam L is the layout type.
	 */
	template<typename T, bool VSA = 1, bool HSA = 0, ConceptLayout L = QVBoxLayout>
	class BaseDockWidget : public BaseWidget<T, QDockWidget>
	{
	  public:
		template<typename... Args>
		BaseDockWidget( Args &&... p_args ) : BaseWidget<T, QDockWidget>( std::forward<Args>( p_args )... )
		{
			// Scroll area.
			if constexpr ( VSA or HSA )
			{
				_scrollArea = new QScrollArea( this );
				QDockWidget::setWidget( _scrollArea );
				_scrollArea->setWidgetResizable( true );
				if constexpr ( VSA )
				{
					_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );
				}
				else
				{
					_scrollArea->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
				}
				if constexpr ( HSA )
				{
					_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
				}
				else
				{
					_scrollArea->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
				}
				_scrollArea->setFrameShape( QFrame::NoFrame );
				_root = new QWidget( _scrollArea );
				_scrollArea->setWidget( _root );
			}
			else
			{
				_root = new QWidget( this );
				QDockWidget::setWidget( _root );
			}

			// Root widget and layout.
			_layout = new L( _root );
			_layout->setContentsMargins( 0, 0, 0, 0 );
			//_layout->setSizeConstraint( QLayout::SetNoConstraint );
		}

		virtual ~BaseDockWidget() = default;

	  protected:
		QPointer<QScrollArea> _scrollArea;
		QPointer<QWidget>	  _root;
		QPointer<L>			  _layout;
	};
} // namespace VTX::UI::QT::Core

#endif
