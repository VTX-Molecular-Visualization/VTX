#ifndef __VTX_UI_QT_WIDGET_BASE_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_WIDGET__

#include <QWidget>
#include <type_traits>
#include <util/hashing.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief A widget must inherit from a QWidget.
	 */
	template<typename W>
	concept ConceptWidget = std::is_base_of_v<QWidget, W>;

	/**
	 * @brief Abstract class that describes a typed widget.
	 * It can be retrieved by its type, and lifetime is logged.
	 */
	template<typename T, ConceptWidget W>
	class BaseWidget : public W
	{
	  public:
		/**
		 * @brief Construct widget and set its object name from its type.
		 */
		explicit BaseWidget( QWidget * p_parent ) : W( p_parent )
		{
			// Set object name.
			const auto name = VTX::Util::typeName<T>();
			W::setObjectName( name );
			VTX_TRACE( "Widget created: {}", name );
		}

		virtual ~BaseWidget() { VTX_TRACE( "Widget deleted: {}", W::objectName().toStdString() ); }

		/**
		 * @brief Check if widget is of type T.
		 */
		template<typename Type>
		bool is() const
		{
			return W::objectName() == VTX::Util::typeName<Type>();
		}
	};

} // namespace VTX::UI::QT::Widget

#endif
