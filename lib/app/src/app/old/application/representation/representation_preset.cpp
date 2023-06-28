#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/application/representation/representation_data_generator.hpp"
#include "app/old/application/setting.hpp"
#include "app/old/component/chemistry/residue.hpp"
#include "app/old/core/event/vtx_event.hpp"
#include "app/old/event.hpp"
#include "app/old/id.hpp"
#include "app/old/vtx_app.hpp"
#include <core/chemdb/residue.hpp>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::App::Old::Application::Representation
{
	namespace ChemDB = VTX::Core::ChemDB;

	RepresentationPreset::RepresentationPreset() : BaseModel( App::Old::ID::Model::MODEL_REPRESENTATION )
	{
		const int randomColorIndex = getId() % int( VTX::Core::ChemDB::Residue::SYMBOL::COUNT );
		_color					   = ChemDB::Residue::SYMBOL_COLOR[ randomColorIndex ];
	}

	RepresentationPreset::RepresentationPreset( const REPRESENTATION_ENUM & p_type ) :
		BaseModel( App::Old::ID::Model::MODEL_REPRESENTATION )
	{
		changeRepresentationType( p_type, false );

		const int randomColorIndex = getId() % int( VTX::Core::ChemDB::Residue::SYMBOL::COUNT );
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
		_notifyViews( App::Old::Event::Model::DISPLAY_NAME_CHANGE );
	}
	void RepresentationPreset::setColor( const Util::Color::Rgba & p_color )
	{
		_color = p_color;
		_notifyDataChanged();
	}

	void RepresentationPreset::setQuickAccess( const bool p_quickAccess )
	{
		_quickAccess = p_quickAccess;
		_notifyViews( App::Old::Event::Model::QUICK_ACCESS_CHANGE );
	}

	void RepresentationPreset::changeRepresentationType( const REPRESENTATION_ENUM & p_newType, const bool p_notify )
	{
		RepresentationData * newData = RepresentationDataGenerator::instantiate( p_newType );

		if ( newData == nullptr )
		{
			VTX_WARNING( "Type {} not managed in Representation::changeRepresentationType.", int( p_newType ) );
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
			_notifyViews( App::Old::Event::Model::REPRESENTATION_TYPE_CHANGE );
		}
	}

	void RepresentationPreset::copyDataFrom( const RepresentationPreset & p_source )
	{
		_data->copyData( *( p_source._data ) );
	}

} // namespace VTX::App::Old::Application::Representation
