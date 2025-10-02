#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__
#define __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__

#include "app/component/chemistry/system.hpp"
#include <core/struct/system.hpp>
#include <renderer/facade.hpp>
#include <renderer/proxy/system.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Render
{
	class ProxySystem : public Core::ECS::BaseComponentProxy<Renderer::Proxy::System>
	{
	  public:
		ProxySystem();
		~ProxySystem();

		void setupProxy() override {}

		void setup( Renderer::Facade & p_renderer );

	  private:
		void _addInRenderer( Renderer::Facade & p_renderer );
		void _setupCallbacks();

		std::vector<uchar> _generateAtomColors( const VTX::Core::Struct::System & p_molStruct ) const;
		std::vector<float> _generateAtomRadii( const VTX::Core::Struct::System & p_molStruct ) const;
		std::vector<uint>  _generateAtomUids( const Component::Chemistry::System & p_molComp ) const;
		std::vector<uchar> _generateResidueColors( const VTX::Core::Struct::System & p_molStruct ) const;
		std::vector<uint>  _generateResidueUids( const Component::Chemistry::System & p_molComp ) const;

		void _applyOnVisibility(
			const Chemistry::IndexRangeList &	   p_rangeList,
			const Chemistry::VISIBILITY_APPLY_MODE p_applyMode
		);

		void _applyVisibilityCallbacks();
		void _applySelectionCallbacks();
		void _applyAtomPositionCallbacks();

		void _removeFromRenderer();
	};

} // namespace VTX::App::Component::Render
#endif
