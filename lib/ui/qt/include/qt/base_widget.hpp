#ifndef __VTX_UI_QT_BASE_WIDGET__
#define __VTX_UI_QT_BASE_WIDGET__

#include <QWidget>
#include <typeinfo>
#include <util/logger.hpp>

namespace VTX::UI::QT
{
	template<typename W>
	concept ConceptWidget = std::derived_from<W, QWidget>;

	template<typename T, ConceptWidget W>
	class BaseWidget : public W
	{
	  public:
		template<typename... Args>
		BaseWidget( Args &&... args ) : W( std::forward<Args>( args )... )
		{
			W::setObjectName( typeid( T ).name() );
		}

	  private:
	};

} // namespace VTX::UI::QT

#endif
