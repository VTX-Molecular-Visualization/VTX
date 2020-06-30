#include "viewpoint.hpp"
#include "view/ui/viewpoint.hpp"

namespace VTX
{
	namespace Model
	{
		void Viewpoint::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() )
			{
				addItem( (View::BaseView<BaseModel> *)Generic::create<Viewpoint, View::UI::Viewpoint>( this ) );
			}
			else
			{
				Generic::destroy( removeItem( ID::View::UI_VIEWPOINT ) );
			}
		}

		void Viewpoint::fromJson( nlohmann::json & ) {}

		nlohmann::json Viewpoint::toJson() const
		{
			nlohmann::json jsonArray = nlohmann::json::array();
			for ( std::string action : _actions )
			{
				std::replace( action.begin(), action.end(), ' ', '-' );
				jsonArray.emplace_back( action );
			}

			return { { "DURATION", _duration },
					 { "POSITION", Util::Math::vecToJson( _position ) },
					 { "TARGET", Util::Math::vecToJson( _target ) },
					 { "ROTATION", Util::Math::quatToJson( _rotation ) },
					 { "DISTANCE", _distance },
					 { "CONTROLLER", _controller },
					 { "ACTIONS", jsonArray } };

			// TODO: add actions.
		}

	} // namespace Model
} // namespace VTX
