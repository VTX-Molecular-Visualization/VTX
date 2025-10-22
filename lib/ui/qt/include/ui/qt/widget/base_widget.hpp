#ifndef __VTX_UI_QT_WIDGET_BASE_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_WIDGET__

#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include <QGuiApplication>
#include <QScreen>
#include <QWidget>
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief A widget must inherit from a QWidget.
	 */
	template<typename W>
	concept ConceptWidget = std::is_base_of_v<QWidget, W>;

	/**
	 * @brief Abstract class that describes a widget behaviour.
	 */
	template<ConceptWidget W>
	class BaseWidget : public W
	{
	  public:
		template<typename... Args>
		BaseWidget( Args &&... p_args ) : W( std::forward<Args>( p_args )... )
		{
			using This		= std::remove_cvref_t<decltype( *this )>;
			const auto name = VTX::Util::typeName<This>();
			W::setObjectName( name );
			VTX_TRACE( "Widget created: {}", name );
		}

		virtual ~BaseWidget()
		{
			using This		= std::remove_cvref_t<decltype( *this )>;
			const auto name = VTX::Util::typeName<This>();
			VTX_TRACE( "Widget deleted: {}", name );
		}

		/**
		 * @brief Hide QWidget::addAction().
		 * @tparam A is the action type.
		 * @return the created QAction.
		 */
		template<ConceptAction A>
		QAction * const addAction()
		{
			QAction * const action = Action::Factory::get<A>();
			QWidget::addAction( action );
			return action;
		}

		void center( const QWidget * const p_w = nullptr )
		{
			// Get geometry of the widget, or screen if not specified.
			QRect geometry = p_w ? p_w->geometry() : QGuiApplication::primaryScreen()->geometry();

			const int x = ( geometry.width() - this->width() ) / 2;
			const int y = ( geometry.height() - this->height() ) / 2;
			this->move( x, y );
		}

		void showEvent( QShowEvent * p_e ) override
		{
			VTX_WARNING( "BaseWidget::showEvent" );
			restore( SETTINGS() );
			W::showEvent( p_e );
		}

		void hideEvent( QHideEvent * p_e ) override
		{
			VTX_WARNING( "BaseWidget::hideEvent" );
			save( SETTINGS() );
			W::hideEvent( p_e );
		}

		virtual void save( Settings & ) {}
		virtual void restore( const Settings & ) {}
	};

} // namespace VTX::UI::QT::Widget

#endif
