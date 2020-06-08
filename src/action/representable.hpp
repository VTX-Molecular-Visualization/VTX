#ifndef __VTX_ACTION_REPRESENTABLE__
#define __VTX_ACTION_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "generic/base_representable.hpp"

namespace VTX
{
	namespace Action
	{
		class BaseActionRepresentable : public BaseAction
		{
		  public:
			explicit BaseActionRepresentable( Generic::BaseRepresentable &	p_representable,
											  const Generic::REPRESENTATION p_representation ) :
				_representable( p_representable ),
				_representation( p_representation )
			{
			}

		  protected:
			Generic::BaseRepresentable &  _representable;
			const Generic::REPRESENTATION _representation;
		};

		class RepresentableAddRepresentation : public BaseActionRepresentable
		{
		  public:
			explicit RepresentableAddRepresentation( Generic::BaseRepresentable &  p_representable,
													 const Generic::REPRESENTATION p_representation ) :
				BaseActionRepresentable( p_representable, p_representation )
			{
			}

			virtual void execute() override { _representable.addRepresentation( _representation ); };
		};

		class RepresentableRemoveRepresentation : public BaseActionRepresentable
		{
		  public:
			explicit RepresentableRemoveRepresentation( Generic::BaseRepresentable &  p_representable,
														const Generic::REPRESENTATION p_representation ) :
				BaseActionRepresentable( p_representable, p_representation )
			{
			}

			virtual void execute() override { _representable.removeRepresentation( _representation ); };
		};
	} // namespace Action
} // namespace VTX
#endif
