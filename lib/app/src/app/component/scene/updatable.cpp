#include "app/component/scene/updatable.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Scene
{
	Updatable::~Updatable()
	{
		for ( const Util::CallbackId id : _callbackIds )
		{
			APP::onUpdate -= id;
		}
	}

	Util::CallbackId Updatable::addUpdateFunction( const UpdateFunction & p_callback )
	{
		Util::CallbackId id = APP::onUpdate += p_callback;
		_callbackIds.emplace_back( id );
		return id;
	}

	void Updatable::removeUpdateFunction( const Util::CallbackId p_id )
	{
		assert( std::ranges::find( _callbackIds, p_id ) != _callbackIds.end() );

		APP::onUpdate -= p_id;
		std::erase( _callbackIds, p_id );
	}

} // namespace VTX::App::Component::Scene
