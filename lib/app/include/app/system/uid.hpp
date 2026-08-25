#ifndef __VTX_APP_SYSTEM_UID__
#define __VTX_APP_SYSTEM_UID__

#include <util/math/range_list.hpp>
#include <util/uid.hpp>

namespace VTX::App::System
{

	struct UID
	{
		/**
		 * @brief List of picking UIDs identifying the elements in the system (residues and atoms).
		 */
		Util::Math::Range<UID32> residues;
		Util::Math::Range<UID32> atoms;

		/**
		 * @brief Get residue index from its picking UID.
		 */
		inline Index getResidueIndex( const UID32 p_uid ) const
		{
			return static_cast<Index>( p_uid - residues.getFirst() );
		}

		/**
		 * @brief Get atom index from its picking UID.
		 */
		inline Index getAtomIndex( const UID32 p_uid ) const { return static_cast<Index>( p_uid - atoms.getFirst() ); }

		/**
		 * @brief Check if the system contains the given residue UID.
		 */
		inline bool containsResidueUID( const UID32 p_uid ) const { return residues.contains( p_uid ); }

		/**
		 * @brief Check if the system contains the given atom UID.
		 */
		inline bool containsAtomUID( const UID32 p_uid ) const { return atoms.contains( p_uid ); }
	};
} // namespace VTX::App::System

#endif
