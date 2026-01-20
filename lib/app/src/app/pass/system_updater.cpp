#include "app/pass/system_updater.hpp"
#include "app/events.hpp"
#include "app/services.hpp"
#include "app/system/color.hpp"
#include "app/system/representation.hpp"
#include "app/system/selection.hpp"
#include "app/system/uid.hpp"
#include "app/system/visibility.hpp"
#include <renderer/renderer.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Pass
{

	SystemUpdater::SystemUpdater()
	{
		auto & reg = REG();

		reg.on_update<System::Visibility>().connect<&SystemUpdater::_onUpdateVisibility>( this );
		reg.on_update<System::Selection>().connect<&SystemUpdater::_onUpdateSelection>( this );
		reg.on_update<System::Representation>().connect<&SystemUpdater::_onUpdateRepresentation>( this );
		reg.on_update<System::Color>().connect<&SystemUpdater::_onUpdateColor>( this );

		HUB().connect<Events::SystemLoad, &SystemUpdater::_onSystemLoaded>( this );
	}

	void SystemUpdater::_onUpdateVisibility( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ visibility, uid, data ] = p_r.get<System::Visibility, System::UID, Core::Struct::System>( p_e );

		std::vector<std::byte> atomVisibility( data.getAtomCount(), std::byte { 0 } );
		for ( const Index i : visibility.atoms )
		{
			atomVisibility[ i ] = std::byte { 1 };
		}

		RENDERER().setSystemVisibility( uid.system, atomVisibility );
	}

	void SystemUpdater::_onUpdateSelection( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ selection, uid, data ] = p_r.get<System::Selection, System::UID, Core::Struct::System>( p_e );

		std::vector<std::byte> atomSelection( data.getAtomCount(), std::byte { 0 } );
		for ( const Index i : selection.atoms )
		{
			atomSelection[ i ] = std::byte { 1 };
		}

		RENDERER().setSystemSelection( uid.system, atomSelection );
	}

	void SystemUpdater::_onUpdateRepresentation( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ representation, uid ] = p_r.get<System::Representation, System::UID>( p_e );
		RENDERER().setSystemRepresentation( uid.system, representation.atoms );
	}

	void SystemUpdater::_onUpdateColor( ECS::Registry & p_r, ECS::Entity p_e )
	{
		const auto & [ color, uid ] = p_r.get<System::Color, System::UID>( p_e );
		RENDERER().setSystemColors( uid.system, color.atoms );
	}

	void SystemUpdater::_onSystemLoaded( const Events::SystemLoad & p_event )
	{
		auto &		reg	   = REG();
		ECS::Entity system = p_event.system;
		assert( std::find( _entities.begin(), _entities.end(), system ) == _entities.end() );
		_entities.push_back( system );

		const auto & data			= reg.get<Core::Struct::System>( system );
		const auto & transform		= reg.get<Util::Math::Transform>( system );
		const auto & uid			= reg.get<System::UID>( system );
		const auto & color			= reg.get<System::Color>( system );
		const auto & representation = reg.get<System::Representation>( system );

		RENDERER().addSystem( uid.system, transform.computeMatrix(), data, uid.atoms.toStdVector() );
		// RENDERER().setSystemRepresentation( uid.system, representation.atoms );
		// RENDERER().setSystemColors( uid.system, color.atoms );
	}
} // namespace VTX::App::Pass
