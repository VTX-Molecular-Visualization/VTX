#ifndef __VTX_UI_QT_BASE_WIDGET__
#define __VTX_UI_QT_BASE_WIDGET__

#include <QWidget>
#include <map>
#include <string>
#include <typeinfo>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	template<typename W>
	concept ConceptWidget = std::is_base_of_v<QWidget, W>;

	// Static map to store all widgets.
	static std::map<std::string, QWidget *> WIDGETS;

	template<ConceptWidget T>
	static T * const WIDGET()
	{
		const std::string name = typeid( T ).name();

		assert( WIDGETS.contains( name ) );

		return static_cast<T *>( WIDGETS[ name ] );
	}

	template<typename T, ConceptWidget W>
	class BaseWidget : public W
	{
	  public:
		template<typename... Args>
		BaseWidget( Args &&... args ) : W( std::forward<Args>( args )... )
		{
			const auto name = typeid( T ).name();
			W::setObjectName( name );
			WIDGETS[ name ] = this;
		}

		virtual ~BaseWidget() { WIDGETS.erase( typeid( T ).name() ); }
	};

} // namespace VTX::UI::QT

#endif
