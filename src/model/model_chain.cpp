#include "model_chain.hpp"
#include "util/type.hpp"
#include "view/view_ui_chain.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelChain::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() ) { addItem( (View::BaseView<BaseModel> *)( new View::ViewUIChain( this ) ) ); }
			else
			{
				_deleteView( ID::View::UI_CHAIN );
			}
		}
	} // namespace Model
} // namespace VTX
