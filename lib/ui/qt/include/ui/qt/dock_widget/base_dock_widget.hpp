#ifndef __VTX_UI_QT_DOCK_WIDGET_BASE_DOCK_WIDGET__
#define __VTX_UI_QT_DOCK_WIDGET_BASE_DOCK_WIDGET__

#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QBoxLayout>
#include <QDockWidget>
#include <QLabel>
#include <QPointer>
#include <QScrollArea>
#include <QToolButton>
#include <concepts>

namespace VTX::UI::QT::DockWidget
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
	class BaseDockWidget : public Widget::BaseWidget<T, QDockWidget>
	{
	  public:
		BaseDockWidget( QWidget * p_parent, QString && p_title ) : Widget::BaseWidget<T, QDockWidget>( p_parent )
		{
			QDockWidget::setWindowTitle( p_title );
			QDockWidget::setMinimumSize( Style::MINIMUM_DOCK_PANEL_WIDTH, Style::MINIMUM_DOCK_PANEL_HEIGHT );

			QDockWidget::setFeatures(
				QDockWidget::features() | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable
			);

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

			_root->setMinimumSize( Style::MINIMUM_DOCK_PANEL_WIDTH, Style::MINIMUM_DOCK_PANEL_HEIGHT );

			// Root widget and layout.
			_layout = new L( _root );
			_layout->setContentsMargins( 0, 0, 0, 0 );
			_layout->setSpacing( 0 );

			//_layout->setSizeConstraint( QLayout::SetNoConstraint );

			// Custom title bar without title.
			/*
			auto * bar = new QWidget( this );
			auto * lay = new QHBoxLayout( bar );
			lay->setContentsMargins( 0, 0, 0, 0 );
			lay->setSpacing( 0 );

			auto * label = new QLabel( p_title, bar );
			label->setAlignment( Qt::AlignVCenter | Qt::AlignLeft );
			label->setAttribute( Qt::WA_TransparentForMouseEvents, true );
			lay->addWidget( label );

			auto * dragArea = new QWidget( bar );
			dragArea->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
			lay->addWidget( dragArea, 1 );

			auto * bFloat = new QToolButton( bar );
			bFloat->setAutoRaise( true );
			bFloat->setIcon( bar->style()->standardIcon( QStyle::SP_TitleBarNormalButton ) );
			QObject::connect(
				bFloat,
				&QToolButton::clicked,
				this,
				[ this, label ] { QDockWidget::setFloating( not QDockWidget::isFloating() ); }
			);
			lay->addWidget( bFloat );

			auto * bClose = new QToolButton( bar );
			bClose->setAutoRaise( true );
			bClose->setIcon( bar->style()->standardIcon( QStyle::SP_TitleBarCloseButton ) );
			QObject::connect( bClose, &QToolButton::clicked, this, &QDockWidget::close );
			lay->addWidget( bClose );

			 QDockWidget::setTitleBarWidget( bar );
			 */
		}

		virtual ~BaseDockWidget() = default;

	  protected:
		QPointer<QScrollArea> _scrollArea;
		QPointer<QWidget>	  _root;
		QPointer<L>			  _layout;
	};
} // namespace VTX::UI::QT::DockWidget

#endif
