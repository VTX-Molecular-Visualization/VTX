#ifndef __VTX_UI_QT_WIDGET_BASE_WIDGET__
#define __VTX_UI_QT_WIDGET_BASE_WIDGET__

#include "ui/qt/actions.hpp"
#include "ui/qt/services.hpp"
#include <QGuiApplication>
#include <QScreen>
#include <QTimer>
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
	 * @brief Savable interface.
	 */
	class ISavable
	{
	  public:
		virtual ~ISavable() = default;

		/**
		 * @brief Save and restore widget settings.
		 */
		virtual void save( Settings & ) {}
		virtual void restore( const Settings & ) {}
	};

	/**
	 * @brief Abstract class that describes a widget behaviour.
	 */
	template<typename T, ConceptWidget W>
	class BaseWidget : public W, public ISavable
	{
	  public:
		/**
		 * @brief Construct widget and set its object name from its type.
		 */
		template<typename... Args>
		BaseWidget( Args &&... p_args ) : W( std::forward<Args>( p_args )... )
		{
			// Set object name.
			const auto name = VTX::Util::typeName<T>();
			W::setObjectName( name );
			VTX_TRACE( "Widget created: {}", name );

			// Restore settings.
			QTimer::singleShot(
				0,
				this,
				[ this ]()
				{
					VTX_WARNING( "BaseWidget::singleShot restore: {}", QObject::objectName().toStdString() );
					restore( SETTINGS() );
				}
			);
		}

		virtual ~BaseWidget() { VTX_TRACE( "Widget deleted: {}", W::objectName().toStdString() ); }

		/**
		 * @brief Hide QWidget::addAction().
		 */
		template<ConceptAction A>
		QAction * const addAction()
		{
			QAction * const action = Action::Factory::get<A>();
			QWidget::addAction( action );
			return action;
		}

		/**
		 * @brief Center the widget on the given widget or on the screen if not specified.
		 */
		void center( const QWidget * const p_w = nullptr )
		{
			// Get geometry of the widget, or screen if not specified.
			QRect geometry = p_w ? p_w->geometry() : QGuiApplication::primaryScreen()->geometry();

			const int x = ( geometry.width() - this->width() ) / 2;
			const int y = ( geometry.height() - this->height() ) / 2;
			this->move( x, y );
		}

	  protected:
		/**
		 * @brief Override close event to save settings.
		 */
		void closeEvent( QCloseEvent * const p_e ) override
		{
			VTX_WARNING( "BaseWidget::closeEvent: {}", QObject::objectName().toStdString() );
			W::closeEvent( p_e );

			save( SETTINGS() );
			VTX_ERROR( "SAVED: {}", W::objectName().toStdString() );

			// Save all children settings.
			for ( QWidget * const child : QWidget::findChildren<QWidget *>() )
			{
				if ( ISavable * const baseWidget = dynamic_cast<ISavable *>( child ) )
				{
					VTX_ERROR( "SAVED: {}", child->objectName().toStdString() );
					baseWidget->save( SETTINGS() );
				}
			}
		}
	};

} // namespace VTX::UI::QT::Widget

#endif
