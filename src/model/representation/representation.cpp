#include "representation.hpp"
#include "all_representation_data.hpp"
#include "color/rgb.hpp"
#include "event/event.hpp"
#include "event/event_manager.hpp"
#include "id.hpp"
#include "model/residue.hpp"
#include "representation_enum.hpp"
#include "setting.hpp"
#include "tool/logger.hpp"
#include "vtx_app.hpp"

namespace VTX::Model::Representation
{
	Representation::Representation() : BaseModel( ID::Model::MODEL_REPRESENTATION )
	{
		const int randomColorIndex = getId() % int( Model::Residue::SYMBOL::COUNT );
		_color					   = Model::Residue::SYMBOL_COLOR[ randomColorIndex ];
	}
	Representation::Representation( const Generic::REPRESENTATION & p_type ) :
		BaseModel( ID::Model::MODEL_REPRESENTATION )
	{
		changeRepresentationType( p_type, false );

		const int randomColorIndex = getId() % int( Model::Residue::SYMBOL::COUNT );
		_color					   = Model::Residue::SYMBOL_COLOR[ randomColorIndex ];
	};

	void Representation::setName( const std::string & p_name )
	{
		_name = std::string( p_name );
		_notifyViews( new Event::VTXEvent( Event::Model::DISPLAY_NAME_CHANGE ) );
	};
	void Representation::setIconPath( const std::string & p_iconPath )
	{
		_iconPath = std::string( p_iconPath );
		_notifyDataChanged();
	};
	void Representation::setColor( const Color::Rgb & p_color )
	{
		_color = p_color;
		_notifyDataChanged();
	}
	void Representation::setQuickAccess( const bool p_quickAccess )
	{
		_quickAccess = p_quickAccess;
		_notifyViews( new Event::VTXEvent( Event::Model::QUICK_ACCESS_CHANGE ) );
	};

	void Representation::changeRepresentationType( const Generic::REPRESENTATION & p_newType, const bool p_notify )
	{
		RepresentationData * newData;

		switch ( p_newType )
		{
		case Generic::REPRESENTATION::STICK: newData = new RepresentationData_Sticks(); break;
		case Generic::REPRESENTATION::STICK_AND_CARTOON: newData = new RepresentationData_StickAndCartoon(); break;
		case Generic::REPRESENTATION::BALL_AND_STICK: newData = new RepresentationData_BallsAndSticks(); break;
		case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
			newData = new RepresentationData_BallStickAndCartoon();
			break;
		case Generic::REPRESENTATION::VAN_DER_WAALS: newData = new RepresentationData_VanDerWaals(); break;
		case Generic::REPRESENTATION::SAS: newData = new RepresentationData_Sas(); break;
		case Generic::REPRESENTATION::TRACE: newData = new RepresentationData_Trace(); break;
		case Generic::REPRESENTATION::CARTOON: newData = new RepresentationData_Cartoon(); break;
		default:
			VTX_WARNING( "Type " + std::to_string( int( p_newType ) )
						 + " not managed in Representation::changeRepresentationType." );
			return;
		}

		if ( _data != nullptr )
		{
			delete _data;
		}

		_data = newData;
		_data->setLinkedRepresentation( this );

		if ( p_notify )
		{
			_notifyViews( new Event::VTXEvent( Event::Model::REPRESENTATION_TYPE_CHANGE ) );
		}
	}

	void Representation::copyDataFrom( const Representation & p_source ) { _data->copyData( *( p_source._data ) ); }

} // namespace VTX::Model::Representation
