#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__
#define __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__

#include "app/component/chemistry/_fwd.hpp"
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

		void addInRenderer( Renderer::Facade & p_renderer );

	  private:
		std::vector<uchar> _generateAtomColors( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<float> _generateAtomRadii( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<uint>  _generateAtomUids( const Component::Chemistry::Molecule & p_molComp ) const;
		std::vector<uchar> _generateResidueColors( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<uint>  _generateResidueUids( const Component::Chemistry::Molecule & p_molComp ) const;

		std::unique_ptr<VTX::Renderer::Proxy::Molecule> _proxyPtr = nullptr;
	};

} // namespace VTX::App::Component::Render
#endif
