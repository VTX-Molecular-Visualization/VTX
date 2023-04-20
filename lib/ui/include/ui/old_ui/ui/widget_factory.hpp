#ifndef __VTX_UI_WIDGETFACTORY__
#define __VTX_UI_WIDGETFACTORY__

#include "widget/base_manual_widget.hpp"
#include "widget/base_manual_widget_initializer.hpp"
#include <QMainWindow>
#include <QTreeWidget>
#include <QWidget>
#include <app/core/mvc/mvc_manager.hpp>
#include <string>
#include <type_traits>

namespace VTX
{
	namespace UI
	{
		class WidgetFactory
		{
		  public:
			inline static WidgetFactory & get()
			{
				static WidgetFactory instance;
				return instance;
			}

		  public:
			template<typename W,
					 typename = std::enable_if<std::is_base_of<Widget::BaseManualWidgetInitializer, W>::value>>
			W * const instantiateWidget( QWidget * const p_parent, const std::string & p_name ) const
			{
				W * const res = new W( p_parent );

				_initManualWidget<W>( res, p_name );

				return res;
			}

			template<typename W,
					 typename P1,
					 typename = std::enable_if<std::is_base_of<Widget::BaseManualWidgetInitializer, W>::value>>
			W * const instantiateWidget( QWidget * const p_parent, P1 p_param1, const std::string & p_name ) const
			{
				// Parent can be optional in most of widget, so we reference it at the end of the params
				W * const res = new W( p_param1, p_parent );

				_initManualWidget<W>( res, p_name );

				return res;
			}

			template<typename V,
					 typename M,
					 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
					 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>,
					 typename = std::enable_if<std::is_base_of<Widget::BaseManualWidgetInitializer, V>::value>>
			V * const instantiateViewWidget( M * const			 p_model,
											 const ID::VTX_ID &	 p_id,
											 QWidget * const	 p_parent,
											 const std::string & p_name ) const
			{
				V * const res = VTX::Core::MVC::MvcManager::get().instantiateViewWidget<V>( p_model, p_id, p_parent );

				_initManualWidget<V>( res, p_name );

				return res;
			}

		  private:
			WidgetFactory() = default;

			template<typename W>
			void _initManualWidget( W * const p_widget, const std::string & p_name ) const
			{
				const QString qstringName = QString::fromStdString( p_name );

				p_widget->blockSignals( true );
				p_widget->setUpdatesEnabled( false );
				p_widget->_setup( qstringName );
				p_widget->setUpdatesEnabled( true );
				p_widget->blockSignals( false );
			}
		};
	} // namespace UI
} // namespace VTX

#endif
