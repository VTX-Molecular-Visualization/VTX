#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__
#define __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__

#include "app/component/chemistry/molecule.hpp"
#include "app/core/visibility/enum.hpp"
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
		std::vector<uint>  _generateAtomUids( const Component::Chemistry::Molecule & p_molComp ) const;
		std::vector<uchar> _generateResidueColors( const VTX::Core::Struct::System & p_molStruct ) const;
		std::vector<uint>  _generateResidueUids( const Component::Chemistry::Molecule & p_molComp ) const;

		void _applyOnVisibility(
			const Component::Chemistry::AtomIndexRangeList & p_rangeList,
			const App::Core::VISIBILITY_APPLY_MODE			 p_applyMode
		);

		void _applyVisibilityCallbacks();
		void _applySelectionCallbacks();
		void _applyAtomPositionCallbacks();

		void _removeFromRenderer();
	};

} // namespace VTX::App::Component::Render
#endif
