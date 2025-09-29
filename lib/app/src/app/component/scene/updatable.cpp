#include "app/component/scene/updatable.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Scene
{
	Updatable::~Updatable() {}

	EventHub::ScopedConnection * Updatable::addUpdateFunction( const UpdateFunction & p_callback )
	{
		_connections.emplace_back(
			std::make_unique<EventHub::ScopedConnection>( HUB().connect<Events::Update>( p_callback ) )
		);
		return _connections.back().get();
	}

	void Updatable::removeUpdateFunction( const EventHub::ScopedConnection * const p_c )
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
