#ifndef __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__

#include "_fwd.hpp"
#include "define.hpp"
#include <util/math/aabb.hpp>

namespace VTX::App::Render
{
	class GPUProxyBuilder;
}

namespace VTX::App::Component::Chemistry
{
	class Molecule : public MoleculeCore
	{
		friend App::Render::GPUProxyBuilder;
		friend Chain;
		friend Residue;
		friend Atom;
		friend Bond;

	  public:
		VTX::Core::Struct::Trajectory & getTrajectory() { return _trajectory; }

		void initAtoms( const size_t p_atomCount );
		void initBonds( const size_t p_bondCount );

		const std::string & getName() const { return _name; }
		void				setName( const std::string & p_name ) { _name = p_name; }

		const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void				setPdbIdCode( const std::string & p_code ) { _pdbIdCode = p_code; }

		void setAtomRadius( const size_t p_index, const float p_radius ) { _atomRadii[ p_index ] = p_radius; }
		void setAtomColor( const size_t p_index, const Util::Color::Rgba & p_color )
		{
			_atomColors[ p_index ] = p_color;
		}

		const Util::Math::AABB & getAABB();

	  protected:
	  private:
		// GPU struct
		Mat4f _transform;
		// std::vector<Vec3f>			   _atomPositions; // Link frame in trajectory
		std::vector<Util::Color::Rgba> _atomColors;
		std::vector<float>			   _atomRadii;
		std::vector<size_t>			   _atomIds;
		std::vector<size_t>			   _bondAtomPairIds;

		std::vector<size_t> _atomVisibilities;
		std::vector<size_t> _atomSelections;

		// App data
		std::string		  _pdbIdCode = "";
		Util::Color::Rgba _color	 = Util::Color::Rgba::randomPastel();
		Util::Math::AABB  _aabb;
	};

} // namespace VTX::App::Component::Chemistry

#endif
