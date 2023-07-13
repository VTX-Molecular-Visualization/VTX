#ifndef __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__

#include "_fwd.hpp"
#include <core/struct/molecule.hpp>
#include <memory>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <vector>

namespace VTX::App::Render
{
	class GPUProxyBuilder;
}

namespace VTX::App::Component::Chemistry
{
	class Molecule
	{
		friend App::Render::GPUProxyBuilder;
		friend Chain;
		friend Residue;
		friend Atom;
		friend Bond;

	  public:
		Molecule();
		Molecule( VTX::Core::Struct::Molecule & p_moleculeStruct );
		~Molecule();

		const VTX::Core::Struct::Molecule & getMoleculeStruct() const { return _moleculeStruct; };
		void								setMoleculeStruct( VTX::Core::Struct::Molecule & p_moleculeStruct );

		void						 initChains( const size_t p_chainCount );
		Chain *						 getChain( const size_t p_chainIndex ) { return _chains[ p_chainIndex ]; }
		const Chain *				 getChain( const size_t p_chainIndex ) const { return _chains[ p_chainIndex ]; }
		std::vector<Chain *> &		 getChains() { return _chains; }
		const std::vector<Chain *> & getChains() const { return _chains; }

		void					 initResidues( const size_t p_residueCount );
		Residue *				 getResidue( const size_t p_chainIndex ) { return _residues[ p_chainIndex ]; }
		const Residue *			 getResidue( const size_t p_chainIndex ) const { return _residues[ p_chainIndex ]; }
		std::vector<Residue *> & getResidues() { return _residues; }
		const std::vector<Residue *> & getResidues() const { return _residues; }

		void						initAtoms( const size_t p_atomCount );
		Atom *						getAtom( const size_t p_chainIndex ) { return _atoms[ p_chainIndex ]; }
		const Atom *				getAtom( const size_t p_chainIndex ) const { return _atoms[ p_chainIndex ]; }
		std::vector<Atom *> &		getAtoms() { return _atoms; }
		const std::vector<Atom *> & getAtoms() const { return _atoms; }

		void						initBonds( const size_t p_bondCount );
		Bond *						getBond( const size_t p_index ) { return _bonds[ p_index ]; }
		const Bond *				getBond( const size_t p_index ) const { return _bonds[ p_index ]; }
		std::vector<Bond *> &		getBonds() { return _bonds; }
		const std::vector<Bond *> & getBonds() const { return _bonds; }

		const std::string & getName() { return _moleculeStruct.name; }
		void				setName( const std::string & p_name ) { _moleculeStruct.name = p_name; }

		VTX::Core::Struct::Trajectory & getTrajectory() { return _moleculeStruct.trajectory; }

		const Util::Math::Transform & getTransform() const { return _transform; }
		Util::Math::Transform &		  getTransform() { return _transform; }

		const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void				setPdbIdCode( const std::string & p_pdbIdCode ) { _pdbIdCode = p_pdbIdCode; }

	  private:
		VTX::Core::Struct::Molecule _moleculeStruct = VTX::Core::Struct::Molecule();

		std::vector<Chain *>   _chains	 = std::vector<Chain *>();
		std::vector<Residue *> _residues = std::vector<Residue *>();
		std::vector<Atom *>	   _atoms	 = std::vector<Atom *>();
		std::vector<Bond *>	   _bonds	 = std::vector<Bond *>();

		Util::Math::Transform _transform = Util::Math::Transform();
		std::string			  _pdbIdCode = "";

		std::vector<uint>			   _atomVisibilities = std::vector<uint>();
		std::vector<Util::Color::Rgba> _atomColors		 = std::vector<Util::Color::Rgba>();
		std::vector<float>			   _atomRadii		 = std::vector<float>();
		std::vector<uint>			   _atomIds			 = std::vector<uint>();
		std::vector<uint>			   _atomSelections	 = std::vector<uint>();
	};

} // namespace VTX::App::Component::Chemistry

#endif