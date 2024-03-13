#ifndef __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__

#include "_fwd.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/core/callback_event.hpp"
#include "app/core/uid/uid.hpp"
#include <core/struct/molecule.hpp>
#include <memory>
#include <util/math/transform.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Render
{
	class ProxyMolecule;
}

namespace VTX::App::Component::Chemistry
{
	class Molecule
	{
	  private:
		// Register component class as component
		inline static const Application::System::ECSSystem::ComponentStaticIDRegistration<Molecule> _compReg {
			"Chemistry::MoleculeComponent"
		};

		friend App::Component::Render::ProxyMolecule;
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

		void		  initChains( const size_t p_chainCount );
		Chain *		  getChain( const size_t p_index ) { return _chains[ p_index ].get(); }
		const Chain * getChain( const size_t p_index ) const { return _chains[ p_index ].get(); }
		std::vector<std::unique_ptr<Chain>> &		getChains() { return _chains; }
		const std::vector<std::unique_ptr<Chain>> & getChains() const { return _chains; }

		void			initResidues( const size_t p_residueCount );
		Residue *		getResidue( const size_t p_index ) { return _residues[ p_index ].get(); }
		const Residue * getResidue( const size_t p_index ) const { return _residues[ p_index ].get(); }
		std::vector<std::unique_ptr<Residue>> &		  getResidues() { return _residues; }
		const std::vector<std::unique_ptr<Residue>> & getResidues() const { return _residues; }

		void		 initAtoms( const size_t p_atomCount );
		Atom *		 getAtom( const atom_index_t p_index ) { return _atoms[ p_index ].get(); }
		const Atom * getAtom( const atom_index_t p_index ) const { return _atoms[ p_index ].get(); }
		std::vector<std::unique_ptr<Atom>> &	   getAtoms() { return _atoms; }
		const std::vector<std::unique_ptr<Atom>> & getAtoms() const { return _atoms; }

		void								 initBonds( const size_t p_bondCount );
		Bond *								 getBond( const size_t p_index ) { return _bonds[ p_index ].get(); }
		const Bond *						 getBond( const size_t p_index ) const { return _bonds[ p_index ].get(); }
		std::vector<std::unique_ptr<Bond>> & getBonds() { return _bonds; }
		const std::vector<std::unique_ptr<Bond>> & getBonds() const { return _bonds; }

		const std::string & getName() const { return _moleculeStruct.name; }
		void				setName( const std::string & p_name );

		bool							hasTrajectory() { return _moleculeStruct.trajectory.frames.size() >= 2; }
		VTX::Core::Struct::Trajectory & getTrajectory() { return _moleculeStruct.trajectory; }

		const Util::Math::Transform & getTransform() const { return _transform; }
		Util::Math::Transform &		  getTransform() { return _transform; }

		const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void				setPdbIdCode( const std::string & p_pdbIdCode ) { _pdbIdCode = p_pdbIdCode; }

		bool getAtomVisibility( const size_t p_index ) const { return _atomVisibilities[ p_index ]; }
		void setAtomVisibility( const size_t p_index, const bool p_visible )
		{
			_atomVisibilities[ p_index ] = p_visible;
		}
		const std::vector<bool> & getAtomVisibilities() const { return _atomVisibilities; }

		const Core::UID::UIDRange & getAtomUIDs() const { return _atomUidRange; }
		const Atom *				getAtomFromUID( Core::UID::uid p_uid ) const;
		Atom *						getAtomFromUID( Core::UID::uid p_uid );

		const Core::UID::UIDRange & getResidueUIDs() const { return _residueUidRange; }
		const Residue *				getResidueFromUID( Core::UID::uid p_uid ) const;
		Residue *					getResidueFromUID( Core::UID::uid p_uid );

		App::Core::CallbackEmitter<> onStructChange = App::Core::CallbackEmitter<>();

	  private:
		VTX::Core::Struct::Molecule _moleculeStruct = VTX::Core::Struct::Molecule();

		std::vector<std::unique_ptr<Chain>>	  _chains;
		std::vector<std::unique_ptr<Residue>> _residues;
		std::vector<std::unique_ptr<Atom>>	  _atoms;
		std::vector<std::unique_ptr<Bond>>	  _bonds;

		Util::Math::Transform _transform = Util::Math::Transform();
		std::string			  _pdbIdCode = "";

		std::vector<bool> _atomVisibilities = std::vector<bool>();
		std::vector<bool> _atomSelections	= std::vector<bool>();

		Core::UID::UIDRange _atomUidRange	 = Core::UID::UIDRange();
		Core::UID::UIDRange _residueUidRange = Core::UID::UIDRange();
	};

} // namespace VTX::App::Component::Chemistry

#endif
