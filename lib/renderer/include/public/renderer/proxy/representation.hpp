#ifndef __VTX_RENDERER_PROXY_REPRESENTATION__
#define __VTX_RENDERER_PROXY_REPRESENTATION__

#include <renderer/representation.hpp>
#include <util/callback.hpp>
#include <util/collection.hpp>

namespace VTX::Renderer::Proxy
{

	/**
	 * @brief Representation data provider.
	 */
	class Representation
	{
	  public:
		const VTX::Renderer::Representation & data;

		Representation( const VTX::Renderer::Representation & p_rep ) : data( p_rep ) {}

		template<E_REPRESENTATION_VALUES S>
		Util::Callback<> & getCallback()
		{
			return *_callbacks.getOrCreateWithHash<Util::Callback<>>( int( S ) );
		}

	  private:
		Util::Collection<std::unique_ptr<Util::Callback<>>> _callbacks;
	};

} // namespace VTX::Renderer::Proxy

#endif
