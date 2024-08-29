#ifndef __VTX_UI_QT_BASE_WIDGET__
#define __VTX_UI_QT_BASE_WIDGET__

#include "actions.hpp"
#include <QGuiApplication>
#include <QScreen>
#include <QWidget>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	/**
	 * @brief A widget must inherit from a QWidget.
	 */
	template<typename W>
	concept ConceptWidget = std::is_base_of_v<QWidget, W>;

	/**
	 * @brief Abstract collection of QWidget pointers.
	 */

	using WIDGET_COLLECTION = VTX::Util::HashedCollection<QWidget *>;

	/**
	 * @brief An accessor to the singleton that store all widgets.
	 */
	using WIDGETS = VTX::Util::Singleton<WIDGET_COLLECTION>;

	/**
	 * @brief Abstract class taht describes a widget behaviour.
	 * @tparam T is the derived class type.
	 * @tparam W is the QWidget type.
	 */
	template<typename T, ConceptWidget W>
	class BaseWidget : public W, public WIDGET_COLLECTION::Registration<T>
	{
	  public:
		template<typename... Args>
		BaseWidget( Args &&... p_args ) :
			W( std::forward<Args>( p_args )... ), WIDGET_COLLECTION::Registration<T>( this )
		{
			const auto name = VTX::Util::typeName<T>();
			W::setObjectName( name );
			VTX_TRACE( "UI widget created: {}", name );
		} // namespace VTX::UI::QT

		virtual ~BaseWidget()
		{
			const auto name = VTX::Util::typeName<T>();
			VTX_TRACE( "UI widget destroyed: {}", name );
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
	};

} // namespace VTX::UI::QT

#endif
