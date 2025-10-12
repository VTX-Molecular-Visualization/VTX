#include "app/component/scene/updatable.hpp"
#include "app/services.hpp"
#include "app/vtx_app.hpp"
#include <util/event_hub.hpp>

namespace VTX::App::Component::Scene
{
	Updatable::~Updatable() {}

	Util::EventHub::ScopedConnection * Updatable::addUpdateFunction( UpdateFunction && p_callback )
	{
		_connections.emplace_back(
			std::make_unique<Util::EventHub::ScopedConnection>(
				HUB().connect<Events::Update>( std::move( p_callback ) )
			)
		);
		return _connections.back().get();
	}

	void Updatable::removeUpdateFunction( const Util::EventHub::ScopedConnection * const p_c )
	{
		auto it = std::find_if(
			_connections.begin(), _connections.end(), [ p_c ]( auto const & uptr ) { return uptr.get() == p_c; }
		);

		if ( it != _connections.end() )
		{
			_connections.erase( it );
		}
	}

} // namespace VTX::App::Component::Scene
