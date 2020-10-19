#include "scene_tree_widget_item.hpp"
#include "style.hpp"
#include <QString>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			void SceneTreeWidgetItem::refreshItem()
			{
				if ( icon( 0 ).isNull() )
					setIcon( 0, *VTX::Style::IconConst::get().getModelSymbol( _linkedModel->getTypeId() ) );

				setCheckState( 0, getCheckState( _linkedModel->isEnable() ) );
				setText( 0, QString::fromStdString( _linkedModel->getDefaultName() ) );
			}
		} // namespace Widget
	}	  // namespace UI

} // namespace VTX
