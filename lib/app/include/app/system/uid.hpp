#ifndef __VTX_APP_SYSTEM_UID__
#define __VTX_APP_SYSTEM_UID__

#include <util/math/range_list.hpp>

namespace VTX::App::System
{

	struct UID
	{
		/**
		 * @brief Root UID identifying the system.
		 */
		RootUID system;

		/**
		 * @brief List of picking UIDs identifying the elements in the system (residues and atoms).
		 */
		Util::Math::Range<PickingUID> residues;
		Util::Math::Range<PickingUID> atoms;

		/**
		 * @brief Get residue index from its picking UID.
		 */
		inline Index getResidueIndex( const PickingUID p_uid ) const
		{
			return static_cast<Index>( p_uid - residues.getFirst() );
		}

		/**
		 * @brief Get atom index from its picking UID.
		 */
		inline Index getAtomIndex( const PickingUID p_uid ) const
		{
			return static_cast<Index>( p_uid - atoms.getFirst() );
		}

		/**
		 * @brief Check if the system contains the given UID.
		 */
		inline bool containsResidueUID( const PickingUID p_uid ) const { return residues.contains( p_uid ); }
		inline bool containsAtomUID( const PickingUID p_uid ) const { return atoms.contains( p_uid ); }
	};
} // namespace VTX::App::System

#endif
