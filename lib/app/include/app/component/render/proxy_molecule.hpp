#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__
#define __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__

#include "app/component/chemistry/molecule.hpp"
#include <app/core/visibility/enum.hpp>
#include <core/struct/molecule.hpp>
#include <memory>
#include <renderer/facade.hpp>
#include <renderer/proxy/molecule.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Render
{
	class ProxyMolecule
	{
	  public:
		ProxyMolecule();
		~ProxyMolecule();

		void setup( Renderer::Facade & p_renderer );

	  private:
		void _addInRenderer( Renderer::Facade & p_renderer );
		void _setupCallbacks();

		std::vector<uchar> _generateAtomColors( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<float> _generateAtomRadii( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<uint>  _generateAtomUids( const Component::Chemistry::Molecule & p_molComp ) const;
		std::vector<uchar> _generateResidueColors( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<uint>  _generateResidueUids( const Component::Chemistry::Molecule & p_molComp ) const;

		void _applyOnVisibility(
			const Component::Chemistry::AtomIndexRangeList & p_rangeList,
			const App::Core::VISIBILITY_APPLY_MODE			 p_applyMode
		);

		std::unique_ptr<VTX::Renderer::Proxy::Molecule> _proxyPtr = nullptr;
	};

} // namespace VTX::App::Component::Render
#endif
