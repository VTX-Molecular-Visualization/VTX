#ifndef __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__

#include "_fwd.hpp"
#include "app/application/ecs/component_registration.hpp"
#include <core/struct/molecule.hpp>
#include <memory>
#include <util/math/aabb.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Render
{
	class GPUProxyBuilder;
}

namespace VTX::App::Component::Chemistry
{
	class Molecule
	{
		// Register component class as component
		inline static const Application::ECS::Registration<Molecule> registration { "Chemistry::MoleculeComponent" };

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
		Chain *						 getChain( const size_t p_index ) { return _chains[ p_index ]; }
		const Chain *				 getChain( const size_t p_index ) const { return _chains[ p_index ]; }
		std::vector<Chain *> &		 getChains() { return _chains; }
		const std::vector<Chain *> & getChains() const { return _chains; }

		void						   initResidues( const size_t p_residueCount );
		Residue *					   getResidue( const size_t p_index ) { return _residues[ p_index ]; }
		const Residue *				   getResidue( const size_t p_index ) const { return _residues[ p_index ]; }
		std::vector<Residue *> &	   getResidues() { return _residues; }
		const std::vector<Residue *> & getResidues() const { return _residues; }

		void						initAtoms( const size_t p_atomCount );
		Atom *						getAtom( const atom_index_t p_index ) { return _atoms[ p_index ]; }
		const Atom *				getAtom( const atom_index_t p_index ) const { return _atoms[ p_index ]; }
		std::vector<Atom *> &		getAtoms() { return _atoms; }
		const std::vector<Atom *> & getAtoms() const { return _atoms; }

		void						initBonds( const size_t p_bondCount );
		Bond *						getBond( const size_t p_index ) { return _bonds[ p_index ]; }
		const Bond *				getBond( const size_t p_index ) const { return _bonds[ p_index ]; }
		std::vector<Bond *> &		getBonds() { return _bonds; }
		const std::vector<Bond *> & getBonds() const { return _bonds; }

		const std::string & getName() const { return _moleculeStruct.name; }
		void				setName( const std::string & p_name );

		bool							hasTrajectory() { return _moleculeStruct.trajectory.frames.size() >= 2; }
		VTX::Core::Struct::Trajectory & getTrajectory() { return _moleculeStruct.trajectory; }

		const Util::Math::Transform & getTransform() const { return _transform; }
		Util::Math::Transform &		  getTransform() { return _transform; }

		const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void				setPdbIdCode( const std::string & p_pdbIdCode ) { _pdbIdCode = p_pdbIdCode; }

		bool getAtomVisibility( const size_t p_index ) const { return bool( _atomVisibilities[ p_index ] ); }
		void setAtomVisibility( const size_t p_index, const bool p_visible )
		{
			_atomVisibilities[ p_index ] = uint( p_visible );
		}
		const std::vector<uint> & getAtomVisibilities() const { return _atomVisibilities; }

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
