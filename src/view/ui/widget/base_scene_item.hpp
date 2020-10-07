#ifndef __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM__
#define __VTX_VIEW_UI_WIDGET_BASE_SCENE_ITEM__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/molecule.hpp"
#include "ui/widget/base_manual_widget_initializer.hpp"
#include "view/base_view.hpp"
#include <QTreeWidget>

namespace VTX
{
	namespace View
	{
		namespace UI
		{
			namespace Widget
			{
				template<typename M>
				// typename = std::enable_if<std::is_base_of<M, Model::BaseModel<>>::value>>
				class BaseSceneItem : public QTreeWidgetItem, public View::BaseView<M>, VTX::UI::Widget::BaseManualWidgetInitializer
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  protected:
					BaseSceneItem( M * const p_model, QTreeWidgetItem * p_parent ) :
						View::BaseView<M>( p_model ), QTreeWidgetItem( p_parent ), VTX::UI::Widget::BaseManualWidgetInitializer() {};
					inline virtual void _setupUi( const QString & p_name ) override
					{
						setCheckState( 0, Qt::CheckState::Checked );
						setText( 0, p_name );
					};
				};

			} // namespace Widget
		}	  // namespace UI
	}		  // namespace View
} // namespace VTX

#endif
