#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <set>

namespace VTX
{
	namespace Generic
	{
		enum class REPRESENTATION : int
		{
			BALL_AND_STICK,
			VAN_DER_WAALS,
			STICK,
			SAS,
			CARTOON,
			COUNT
		};

		class BaseRepresentable
		{
		  public:
			inline const std::set<REPRESENTATION> & getRepresentations() const { return _representations; }
			void									addRepresentation( const REPRESENTATION p_representation )
			{
				_representations.emplace( p_representation );
			};
			void removeRepresentation( const REPRESENTATION p_representation )
			{
				_representations.erase( p_representation );
			};

		  protected:
			std::set<REPRESENTATION> _representations = std::set<REPRESENTATION>();
		};
	} // namespace Generic
} // namespace VTX
#endif
