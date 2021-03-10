#ifndef __VTX_MODEL_REPRESENTATION_ENUM__
#define __VTX_MODEL_REPRESENTATION_ENUM__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Representation
	{
		enum class FlagDataTargeted : int
		{
			NONE   = 0,
			ATOM   = 1 << 0,
			BOND   = 1 << 1,
			RIBBON = 1 << 2,
		};

		inline FlagDataTargeted operator|( const FlagDataTargeted p_lhs, const FlagDataTargeted p_rhs )
		{
			return static_cast<FlagDataTargeted>( static_cast<int>( p_lhs ) | static_cast<int>( p_rhs ) );
		}
		inline FlagDataTargeted & operator|=( FlagDataTargeted & lhs, const FlagDataTargeted rhs )
		{
			lhs = lhs | rhs;
			return lhs;
		}
		inline FlagDataTargeted operator&( const FlagDataTargeted lhs, const FlagDataTargeted rhs )
		{
			return static_cast<FlagDataTargeted>( static_cast<int>( lhs ) & static_cast<int>( rhs ) );
		}
		inline FlagDataTargeted & operator&=( FlagDataTargeted & lhs, const FlagDataTargeted rhs )
		{
			lhs = lhs & rhs;
			return lhs;
		}

		enum class MoleculeComputationFlag : int
		{
			Targets			   = 1 << 0,
			ColorBuffer		   = 1 << 1,
			SecondaryStructure = 1 << 2,
			ALL				   = 0xFFFF,
		};

		inline MoleculeComputationFlag operator&( const MoleculeComputationFlag p_lhs,
												  const MoleculeComputationFlag p_rhs )
		{
			return static_cast<MoleculeComputationFlag>( int( p_lhs ) & int( p_rhs ) );
		}
		inline MoleculeComputationFlag operator&=( MoleculeComputationFlag & lhs, const MoleculeComputationFlag rhs )
		{
			lhs = lhs & rhs;
			return lhs;
		}
		inline MoleculeComputationFlag operator|( const MoleculeComputationFlag lhs, const MoleculeComputationFlag rhs )
		{
			return static_cast<MoleculeComputationFlag>( int( lhs ) | int( rhs ) );
		}
		inline MoleculeComputationFlag operator|=( MoleculeComputationFlag & lhs, const MoleculeComputationFlag rhs )
		{
			lhs = ( MoleculeComputationFlag )( int( lhs ) | int( rhs ) );
			return lhs;
		};
	} // namespace Representation
	namespace Generic
	{
		enum class REPRESENTATION : int
		{
			BALL_AND_STICK,
			VAN_DER_WAALS,
			STICK,
			TRACE,
			SAS,
			CARTOON,
			BALL_AND_STICK_AND_CARTOON,
			STICK_AND_CARTOON,
			COUNT
		};
	} // namespace Generic
} // namespace VTX
#endif
