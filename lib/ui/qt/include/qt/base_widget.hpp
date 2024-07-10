#ifndef __VTX_UI_QT_BASE_WIDGET__
#define __VTX_UI_QT_BASE_WIDGET__

#include "actions.hpp"
#include <QWidget>
#include <map>
#include <string>
#include <typeinfo>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	template<typename W>
	concept ConceptWidget = std::is_base_of_v<QWidget, W>;

	static std::map<std::string, QWidget *> WIDGETS;

	template<ConceptWidget T>
	static T * const WIDGET()
	{
		const std::string name = typeid( T ).name();

		assert( WIDGETS.contains( name ) );

		return static_cast<T * const>( WIDGETS[ name ] );
	}

	template<typename T, ConceptWidget W>
	class BaseWidget : public W
	{
	  public:
		template<typename... Args>
		BaseWidget( Args &&... p_args ) : W( std::forward<Args>( p_args )... )
		{
			const std::string name = typeid( T ).name();
			W::setObjectName( name );
			WIDGETS[ name ] = this;
			VTX_TRACE( "Widget created: {}", name );
		}

		virtual ~BaseWidget() { WIDGETS.erase( typeid( T ).name() ); }

		// Hide QWidget::addAction().
		template<ConceptAction T>
		QAction * const addAction()
		{
			QAction * const action = ACTION<T>();
			QWidget::addAction( action );
			return action;
		}
	};

} // namespace VTX::UI::QT

#endif
