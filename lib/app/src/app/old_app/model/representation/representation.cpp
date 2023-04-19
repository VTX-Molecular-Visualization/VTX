#include "app/old_app/model/representation/representation.hpp"
#include "app/old_app/color/rgba.hpp"
#include "app/event/vtx_event.hpp"
#include "app/core/event/event_manager.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/model/representation/all_representation_data.hpp"
#include "app/old_app/model/representation/representation_enum.hpp"
#include "app/old_app/model/residue.hpp"
#include "app/old_app/setting.hpp"
#include "app/old_app/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::Model::Representation
{
	Representation::Representation() : BaseModel( VTX::ID::Model::MODEL_REPRESENTATION )
	{
		const int randomColorIndex = getId() % int( Model::Residue::SYMBOL::COUNT );
		_color					   = Model::Residue::SYMBOL_COLOR[ randomColorIndex ];
	}

	Representation::Representation( const Generic::REPRESENTATION & p_type ) :
		BaseModel( VTX::ID::Model::MODEL_REPRESENTATION )
	{
		changeRepresentationType( p_type, false );

		const int randomColorIndex = getId() % int( Model::Residue::SYMBOL::COUNT );
		_color					   = Model::Residue::SYMBOL_COLOR[ randomColorIndex ];
	}

	Representation::~Representation()
	{
		if ( _data != nullptr )
		{
			delete _data;
		}
	}

	void Representation::setName( const std::string & p_name )
	{
		_name = std::string( p_name );
		_notifyViews( new Event::VTXEvent( Event::Model::DISPLAY_NAME_CHANGE ) );
	}
	void Representation::setColor( const Color::Rgba & p_color )
	{
		_color = p_color;
		_notifyDataChanged();
	}

	void Representation::setQuickAccess( const bool p_quickAccess )
	{
		_quickAccess = p_quickAccess;
		_notifyViews( new Event::VTXEvent( Event::Model::QUICK_ACCESS_CHANGE ) );
	}

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
		case Generic::REPRESENTATION::SAS:
			newData = new RepresentationData_Sas();
			break;
			// !V0.1
			// case Generic::REPRESENTATION::TRACE: newData = new RepresentationData_Trace(); break;
		case Generic::REPRESENTATION::CARTOON: newData = new RepresentationData_Cartoon(); break;
		case Generic::REPRESENTATION::SES: newData = new RepresentationData_Ses(); break;
		default:
			VTX_WARNING( "Type " + std::to_string( int( p_newType ) )
						 + " not managed in Representation::changeRepresentationType." );
			newData = new RepresentationData_Sticks();
			break;
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
