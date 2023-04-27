#include "app/application/representation/representation_preset.hpp"
#include "app/application/representation/representation_data_generator.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/event.hpp"
#include "app/internal/chemdb/residue.hpp"
#include "app/old_app/color/rgba.hpp"
#include "app/old_app/id.hpp"
#include "app/application/setting.hpp"
#include "app/old_app/vtx_app.hpp"
#include <util/logger.hpp>

namespace VTX::App::Application::Representation
{
	namespace ChemDB = VTX::App::Internal::ChemDB;

	RepresentationPreset::RepresentationPreset() : BaseModel( VTX::ID::Model::MODEL_REPRESENTATION )
	{
		const int randomColorIndex = getId() % int( App::Internal::ChemDB::Residue::SYMBOL::COUNT );
		_color					   = ChemDB::Residue::SYMBOL_COLOR[ randomColorIndex ];
	}

	RepresentationPreset::RepresentationPreset( const REPRESENTATION_ENUM & p_type ) :
		BaseModel( VTX::ID::Model::MODEL_REPRESENTATION )
	{
		changeRepresentationType( p_type, false );

		const int randomColorIndex = getId() % int( App::Internal::ChemDB::Residue::SYMBOL::COUNT );
		_color					   = ChemDB::Residue::SYMBOL_COLOR[ randomColorIndex ];
	}

	RepresentationPreset::~RepresentationPreset()
	{
		if ( _data != nullptr )
		{
			delete _data;
		}
	}

	void RepresentationPreset::setName( const std::string & p_name )
	{
		_name = std::string( p_name );
		_notifyViews( App::Event::Model::DISPLAY_NAME_CHANGE );
	}
	void RepresentationPreset::setColor( const Color::Rgba & p_color )
	{
		_color = p_color;
		_notifyDataChanged();
	}

	void RepresentationPreset::setQuickAccess( const bool p_quickAccess )
	{
		_quickAccess = p_quickAccess;
		_notifyViews( App::Event::Model::QUICK_ACCESS_CHANGE );
	}

	void RepresentationPreset::changeRepresentationType( const REPRESENTATION_ENUM & p_newType, const bool p_notify )
	{
		RepresentationData * newData = RepresentationDataGenerator::instantiate( p_newType );

		if ( newData == nullptr )
		{
			VTX_WARNING( "Type " + std::to_string( int( p_newType ) )
						 + " not managed in Representation::changeRepresentationType." );
			newData = RepresentationDataGenerator::instantiate( REPRESENTATION_ENUM::STICK );
		}

		if ( _data != nullptr )
		{
			delete _data;
		}

		_data = newData;
		_data->setLinkedRepresentation( this );

		if ( p_notify )
		{
			_notifyViews( App::Event::Model::REPRESENTATION_TYPE_CHANGE );
		}
	}

	void RepresentationPreset::copyDataFrom( const RepresentationPreset & p_source )
	{
		_data->copyData( *( p_source._data ) );
	}

} // namespace VTX::App::Application::Representation
