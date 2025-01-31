#ifndef __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__

#include "_fwd.hpp"
#include "app/component/chemistry/index_types.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/core/uid/uid.hpp"
#include <app/core/visibility/enum.hpp>
#include <core/struct/system.hpp>
#include <memory>
#include <util/callback.hpp>
#include <util/math/range_list.hpp>
#include <util/math/transform.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Render
{
	class ProxySystem;
}

namespace VTX::App::Component::Chemistry
{
	class System : public Core::ECS::BaseComponent
	{
	  private:
		friend App::Component::Render::ProxySystem;
		friend Chain;
		friend Residue;
		friend Atom;
		friend Bond;

	  public:
		System();
		System( VTX::Core::Struct::System & p_systemStruct );
		~System();

		const VTX::Core::Struct::System & getSystemStruct() const { return _systemStruct; };
		VTX::Core::Struct::System &		  getSystemStruct() { return _systemStruct; };
		void							  setSystemStruct( VTX::Core::Struct::System & p_systemStruct );

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

		const std::string & getName() const { return _systemStruct.name; }
		void				setName( const std::string & p_name );

		bool hasTrajectory() { return _systemStruct.trajectory.getCurrentFrame().size() >= 1; }
		VTX::Core::Struct::Trajectory & getTrajectory() { return _systemStruct.trajectory; }

		const Util::Math::Transform & getTransform() const { return _transform; }
		Util::Math::Transform &		  getTransform() { return _transform; }

		const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void				setPdbIdCode( const std::string & p_pdbIdCode ) { _pdbIdCode = p_pdbIdCode; }

		bool isVisible() const;
		bool isFullyVisible() const;

		void setVisible( const bool p_visible );
		void setVisible( const atom_index_t & p_atomId, bool p_visible );
		void setVisible( const AtomIndexRange & p_atomRange, bool p_visible );
		void setVisible( const AtomIndexRangeList & p_atomRangeList, bool p_visible );

		void remove( const atom_index_t & p_atomIndex );
		void remove( const AtomIndexRange & p_atomRange );
		void remove( const AtomIndexRangeList & p_atomRangeList );

		size_t getRealChainCount() const { return _realChainCount; }
		size_t getRealResidueCount() const { return _realResidueCount; }
		size_t getRealAtomCount() const { return _realAtomCount; };

		const AtomIndexRangeList & getAtomVisibilities() const { return _visibleAtomIds; }
		void					   setAtomVisibilities( const AtomIndexRangeList & p_visibility );

		const AtomIndexRangeList & getActiveAtoms() const { return _activeAtomIds; }

		const Core::UID::UIDRange & getAtomUIDs() const { return _atomUidRange; }
		const Atom *				getAtomFromUID( Core::UID::uid p_uid ) const;

		Atom * getAtomFromUID( Core::UID::uid p_uid );

		const Core::UID::UIDRange & getResidueUIDs() const { return _residueUidRange; }
		const Residue *				getResidueFromUID( Core::UID::uid p_uid ) const;
		Residue *					getResidueFromUID( Core::UID::uid p_uid );

		Util::Callback<>													 onStruct;
		Util::Callback<AtomIndexRangeList, App::Core::VISIBILITY_APPLY_MODE> onVisibilityChange;
		Util::Callback<AtomIndexRangeList>									 onAtomRemoved;
		Util::Callback<>													 onTrajectoryAdded;

	  private:
		void _deleteTopologyPointers( const atom_index_t p_atomIndex );
		void _deleteTopologyPointers( const AtomIndexRange & p_atomRange );
		void _refreshResidueRemovedState( const size_t p_residueIndex );
		void _refreshChainRemovedState( const size_t p_chainIndex );

		void _internalDeleteAtom( const atom_index_t p_index );
		void _internalDeleteAtoms( const AtomIndexRange & p_range );
		void _internalDeleteResidue( const size_t p_index );
		void _internalDeleteResidues( const ResidueIndexRange & p_range );
		void _internalDeleteChain( const size_t p_index );
		void _internalDeleteChains( const ChainIndexRange & p_range );

		void _resizeTopologyVectors();

		VTX::Core::Struct::System _systemStruct = VTX::Core::Struct::System();

		std::vector<std::unique_ptr<Chain>>	  _chains;
		std::vector<std::unique_ptr<Residue>> _residues;
		std::vector<std::unique_ptr<Atom>>	  _atoms;
		std::vector<std::unique_ptr<Bond>>	  _bonds;

		size_t _realChainCount;
		size_t _realResidueCount;
		size_t _realAtomCount;

		Util::Math::Transform _transform;
		std::string			  _pdbIdCode = "";

		AtomIndexRangeList _visibleAtomIds;
		AtomIndexRangeList _activeAtomIds;

		Core::UID::UIDRange _atomUidRange;
		Core::UID::UIDRange _residueUidRange;
	};

} // namespace VTX::App::Component::Chemistry

#endif
