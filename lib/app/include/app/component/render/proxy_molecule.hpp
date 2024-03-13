#ifndef __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__
#define __VTX_APP_COMPONENT_RENDER_PROXY_MOLECULE__

#include "app/component/chemistry/molecule.hpp"
#include <core/struct/molecule.hpp>
#include <memory>
#include <renderer/proxy/molecule.hpp>
#include <util/callback.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Render
{
	class ProxyMolecule
	{
	  public:
		ProxyMolecule();
		void init();

	  private:
		std::vector<uchar> generateAtomColors( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<float> generateAtomRadii( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<uint>  generateAtomUids( const Component::Chemistry::Molecule & p_molStruct ) const;
		std::vector<uchar> generateResidueColors( const VTX::Core::Struct::Molecule & p_molStruct ) const;
		std::vector<uint>  generateResidueUids( const Component::Chemistry::Molecule & p_molStruct ) const;

		std::unique_ptr<VTX::Renderer::Proxy::Molecule> _proxyPtr = nullptr;
	};

} // namespace VTX::App::Component::Render
#endif
