#ifndef __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__
#define __VTX_APP_COMPONENT_CHEMISTRY_MOLECULE__

#include "_fwd.hpp"
#include "app/core/ecs/ecs_system.hpp"
#include "app/uid/uid.hpp"
#include "index_types.hpp"
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
	enum class VISIBILITY_APPLY_MODE
	{
		SHOW,
		HIDE,
		SET
	};

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

		void								  initChains( const Index p_chainCount );
		Chain *								  getChain( const Index p_index ) { return _chains[ p_index ].get(); }
		const Chain *						  getChain( const Index p_index ) const { return _chains[ p_index ].get(); }
		std::vector<std::unique_ptr<Chain>> & getChains() { return _chains; }
		const std::vector<std::unique_ptr<Chain>> & getChains() const { return _chains; }

		void			initResidues( const Index p_residueCount );
		Residue *		getResidue( const Index p_index ) { return _residues[ p_index ].get(); }
		const Residue * getResidue( const Index p_index ) const { return _residues[ p_index ].get(); }
		std::vector<std::unique_ptr<Residue>> &		  getResidues() { return _residues; }
		const std::vector<std::unique_ptr<Residue>> & getResidues() const { return _residues; }

		void								 initAtoms( const Index p_atomCount );
		Atom *								 getAtom( const Index p_index ) { return _atoms[ p_index ].get(); }
		const Atom *						 getAtom( const Index p_index ) const { return _atoms[ p_index ].get(); }
		std::vector<std::unique_ptr<Atom>> & getAtoms() { return _atoms; }
		const std::vector<std::unique_ptr<Atom>> & getAtoms() const { return _atoms; }

		void								 initBonds( const Index p_bondCount );
		Bond *								 getBond( const Index p_index ) { return _bonds[ p_index ].get(); }
		const Bond *						 getBond( const Index p_index ) const { return _bonds[ p_index ].get(); }
		std::vector<std::unique_ptr<Bond>> & getBonds() { return _bonds; }
		const std::vector<std::unique_ptr<Bond>> & getBonds() const { return _bonds; }

		const std::string & getName() const { return _systemStruct.name; }
		void				setName( const std::string & p_name );

		bool							hasTrajectory() { return _systemStruct.trajectory.getFrameCount() > 1; }
		VTX::Core::Struct::Trajectory & getTrajectory() { return _systemStruct.trajectory; }

		const Util::Math::Transform & getTransform() const { return _transform; }
		Util::Math::Transform &		  getTransform() { return _transform; }

		const std::string & getPdbIdCode() const { return _pdbIdCode; }
		void				setPdbIdCode( const std::string & p_pdbIdCode ) { _pdbIdCode = p_pdbIdCode; }

		bool isVisible() const;
		bool isFullyVisible() const;

		void setVisible( const bool p_visible );
		void setVisible( const Index & p_atomId, bool p_visible );
		void setVisible( const IndexRange & p_atomRange, bool p_visible );
		void setVisible( const IndexRangeList & p_atomRangeList, bool p_visible );

		void remove( const Index & p_atomIndex );
		void remove( const IndexRange & p_atomRange );
		void remove( const IndexRangeList & p_atomRangeList );

		Index getRealChainCount() const { return _realChainCount; }
		Index getRealResidueCount() const { return _realResidueCount; }
		Index getRealAtomCount() const { return _realAtomCount; };

		const IndexRangeList & getAtomVisibilities() const { return _visibleAtomIds; }
		void				   setAtomVisibilities( const IndexRangeList & p_visibility );

		const IndexRangeList & getActiveAtoms() const { return _activeAtomIds; }

		const Uid::UIDRange & getAtomUIDs() const { return _atomUidRange; }
		const Atom *		  getAtomFromUID( Uid::uid p_uid ) const;

		Atom * getAtomFromUID( Uid::uid p_uid );

		const Uid::UIDRange & getResidueUIDs() const { return _residueUidRange; }
		const Residue *		  getResidueFromUID( Uid::uid p_uid ) const;
		Residue *			  getResidueFromUID( Uid::uid p_uid );

		Util::Callback<>									  onStruct;
		Util::Callback<IndexRangeList, VISIBILITY_APPLY_MODE> onVisibilityChange;
		Util::Callback<IndexRangeList>						  onAtomRemoved;
		Util::Callback<>									  onTrajectoryAdded;

	  private:
		void _deleteTopologyPointers( const Index p_atomIndex );
		void _deleteTopologyPointers( const IndexRange & p_atomRange );
		void _refreshResidueRemovedState( const Index p_residueIndex );
		void _refreshChainRemovedState( const Index p_chainIndex );

		void _internalDeleteAtom( const Index p_index );
		void _internalDeleteAtoms( const IndexRange & p_range );
		void _internalDeleteResidue( const Index p_index );
		void _internalDeleteResidues( const IndexRange & p_range );
		void _internalDeleteChain( const Index p_index );
		void _internalDeleteChains( const IndexRange & p_range );

		void _resizeTopologyVectors();

		VTX::Core::Struct::System _systemStruct = VTX::Core::Struct::System();

		std::vector<std::unique_ptr<Chain>>	  _chains;
		std::vector<std::unique_ptr<Residue>> _residues;
		std::vector<std::unique_ptr<Atom>>	  _atoms;
		std::vector<std::unique_ptr<Bond>>	  _bonds;

		Index _realChainCount;
		Index _realResidueCount;
		Index _realAtomCount;

		Util::Math::Transform _transform;
		std::string			  _pdbIdCode = "";

		IndexRangeList _visibleAtomIds;
		IndexRangeList _activeAtomIds;

		Uid::UIDRange _atomUidRange;
		Uid::UIDRange _residueUidRange;
	};

} // namespace VTX::App::Component::Chemistry

#endif
