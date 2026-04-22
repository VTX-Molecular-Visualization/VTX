#ifndef __VTX_UI_QT_WIDGET_BASE_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_WIDGET__

#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
#include <QWidget>
#include <string_view>
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
	 * @brief Abstract class that describes a widget behaviour.
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
		 * @brief Center the widget on the given widget or on the screen if not specified.
		 */
		void center( const QWidget * const p_w = nullptr )
		{
			// Get geometry of the widget, or screen if not specified.
			QRect geometry = p_w ? p_w->geometry() : QGuiApplication::primaryScreen()->availableGeometry();

			const int x = ( geometry.width() - this->width() ) / 2;
			const int y = ( geometry.height() - this->height() ) / 2;
			this->move( x, y );
		}

		/**
		 * @brief Hide QWidget::addAction(). Link registered action to this widget.
		 */
		QAction * const addAction( const std::string_view p_actionId )
		{
			QAction * const action = UI_ACTIONS().getAction( p_actionId );
			if ( action == nullptr )
			{
				VTX_ERROR( "Unable to add unregistered UI action to widget: {}", p_actionId );
				return nullptr;
			}

			QWidget::addAction( action );
			return action;
		}

	};

} // namespace VTX::UI::QT::Widget

#endif
