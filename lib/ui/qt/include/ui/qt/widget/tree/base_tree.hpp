#pragma once

#include "ui/qt/widget/base_widget.hpp"
#include <QApplication>
#include <QEvent>
#include <QTreeWidget>
#include <optional>

namespace VTX::UI::QT::Widget::Tree
{
	/**
	 * @brief Base class for tree widgets that auto adjust their height to their content (only one top level item).
	 */
	template<typename T, ConceptWidget W = QTreeWidget>
	class BaseTree : public Widget::BaseWidget<T, W>
	{
	  public:
		~BaseTree() override = default;

		/**
		 * @brief Override size hints to adapt height to content.
		 */
		QSize minimumSizeHint() const override { return QSize( 0, 0 ); }

		QSize sizeHint() const override { return QSize( QAbstractScrollArea::sizeHint().width(), _contentHeight() ); }

		/**
		 * @brief Set the tint color.
		 */
		void setTint( const int p_hue, const int p_lightness = DEFAULT_TINT_LIGHTNESS )
		{
			_tintHue	   = p_hue;
			_tintLightness = p_lightness;
			_applyTint();
		}

		bool event( QEvent * const p_event ) override
		{
			const bool result = W::event( p_event );
			if ( ( p_event->type() == QEvent::ApplicationPaletteChange || p_event->type() == QEvent::PaletteChange )
				 && not _applyingTint )
			{
				_applyTint();
			}
			return result;
		}

	  protected:
		/**
		 * @brief Keep the current selection when clicking a non selectable item.
		 */
		QItemSelectionModel::SelectionFlags selectionCommand(
			const QModelIndex &	 p_index,
			const QEvent * const p_event
		) const override
		{
			if ( p_index.isValid() && not( W::model()->flags( p_index ) & Qt::ItemIsSelectable ) )
			{
				return QItemSelectionModel::NoUpdate;
			}

			return W::selectionCommand( p_index, p_event );
		}

		/**
		 * @brief Default UI settings.
		 */
		BaseTree( QWidget * const p_parent ) : Widget::BaseWidget<T, W>( p_parent )
		{
			W::setHeaderHidden( true );
			W::setItemsExpandable( true );
			W::setAllColumnsShowFocus( true );
			W::setUniformRowHeights( true );
			W::setExpandsOnDoubleClick( true );
			W::setIndentation( INDENTATION );
			W::setEditTriggers( QAbstractItemView::NoEditTriggers );
			W::setFrameStyle( QFrame::NoFrame );
			W::setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Minimum );
			W::setMinimumHeight( 0 );
			W::setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
			W::setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

			W::connect( this, &QTreeView::expanded, this, [ this ]( const QModelIndex & ) { W::updateGeometry(); } );
			W::connect( this, &QTreeView::collapsed, this, [ this ]( const QModelIndex & ) { W::updateGeometry(); } );
		}

	  private:
		static constexpr int INDENTATION			= 10;
		static constexpr int DEFAULT_TINT_LIGHTNESS = 128;

		/**
		 * @brief Tint color hue.
		 */
		std::optional<int> _tintHue;
		int				   _tintLightness = DEFAULT_TINT_LIGHTNESS;
		bool			   _applyingTint  = false;

		/**
		 * @brief Apply the tint color to the tree.
		 */
		void _applyTint()
		{
			if ( not _tintHue )
			{
				return;
			}

			const QPalette appPalette = QApplication::palette();
			const QColor   base		  = appPalette.color( QPalette::Base );
			const bool	   isDark	  = base.lightness() < 128;
			const QColor   hue		  = QColor::fromHsl( *_tintHue, 200, _tintLightness );
			const float	   strength	  = isDark ? 0.12f : 0.08f;

			const auto mix = [ strength ]( const int p_a, const int p_b )
			{ return static_cast<int>( p_a + ( p_b - p_a ) * strength ); };

			const QColor tinted(
				mix( base.red(), hue.red() ), mix( base.green(), hue.green() ), mix( base.blue(), hue.blue() )
			);

			QPalette pal = W::palette();
			pal.setColor( QPalette::Base, tinted );
			_applyingTint = true;
			W::setPalette( pal );
			_applyingTint = false;
		}

		/**
		 * @brief Calculate the content height of the tree.
		 */
		int _contentHeight() const
		{
			const int rowHeight = this->sizeHintForRow( 0 );
			if ( rowHeight <= 0 )
			{
				return 0;
			}

			const QAbstractItemModel * const m = this->model();
			if ( m == nullptr )
			{
				return 0;
			}

			int rows = 0;

			const QModelIndex root = this->rootIndex();

			std::function<void( const QModelIndex & )> countRows = [ & ]( const QModelIndex & parent )
			{
				const int rc = m->rowCount( parent );
				for ( int r = 0; r < rc; ++r )
				{
					const QModelIndex idx = m->index( r, 0, parent );
					if ( not idx.isValid() )
					{
						continue;
					}

					++rows;

					if ( this->isExpanded( idx ) )
					{
						countRows( idx );
					}
				}
			};

			countRows( root );

			return rows * rowHeight + 2 * this->frameWidth();
		}
	};
} // namespace VTX::UI::QT::Widget::Tree
