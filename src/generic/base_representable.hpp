#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

namespace VTX
{
	namespace Generic
	{
		enum class REPRESENTATION : int
		{
			INHERITED,
			BALL_AND_STICK,
			VAN_DER_WAALS,
			STICK,
			SAS
		};

		class BaseRepresentable
		{
		  public:
			virtual ~BaseRepresentable() = default;

			inline const REPRESENTATION getRepresentation() const { return _representation; }
			virtual void				setRepresentation( const REPRESENTATION p_representation )
			{
				_representation = p_representation;
			};

		  protected:
			REPRESENTATION _representation = REPRESENTATION::INHERITED;
		};
	} // namespace Generic
} // namespace VTX
#endif
