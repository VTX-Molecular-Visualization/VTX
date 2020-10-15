#include "scene_tree_widget_item.hpp"
#include <QString>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			void SceneTreeWidgetItem::fillData( const Model::BaseModel & p_model, const std::string & p_label, const QIcon & p_icon )
			{
				setIcon( 0, p_icon );
				setCheckState( 0, getCheckState( p_model ) );
				setText( 0, QString::fromStdString( p_label ) );
			}

			Qt::CheckState SceneTreeWidgetItem::getCheckState( const Model::BaseModel & p_model )
			{
				return p_model.isEnable() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked;
			}
		} // namespace Widget
	}	  // namespace UI

} // namespace VTX
