#ifndef __VTX_UI_QT_BASE_WIDGET__
#define __VTX_UI_QT_BASE_WIDGET__

#include "actions.hpp"
#include <QGuiApplication>
#include <QScreen>
#include <QWidget>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	template<typename W>
	concept ConceptWidget = std::is_base_of_v<QWidget, W>;

	// TODO: use static singleton, global variable, or static member?
	// inline Util::HashedCollection<QWidget *> WIDGETS;

	using WIDGET_COLLECTION = Util::HashedCollection<QWidget *>;
	using WIDGETS			= Util::Singleton<WIDGET_COLLECTION>;

	template<typename T, ConceptWidget W>
	class BaseWidget : public W, public WIDGET_COLLECTION::Registration<T>
	{
	  public:
		template<typename... Args>
		// TODO: auto registrate in Collection.
		BaseWidget( Args &&... p_args ) :
			W( std::forward<Args>( p_args )... ), WIDGET_COLLECTION::Registration<T>( this )
		{
			const std::string name = typeid( T ).name();
			W::setObjectName( name );
			// WIDGETS::get().set<T>( this );
			VTX_TRACE( "Widget created: {}", name );
		} // namespace VTX::UI::QT

		// TODO: fix this, maybe widget is not in the collection anymore.
		virtual ~BaseWidget()
		{
			// WIDGETS::get().remove<T>();
		}

		// Hide QWidget::addAction().
		template<ConceptAction T>
		QAction * const addAction()
		{
			QAction * const action = Action::Factory::get<T>();
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
