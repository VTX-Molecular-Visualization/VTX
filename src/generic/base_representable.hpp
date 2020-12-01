#ifndef __VTX_BASE_REPRESENTABLE__
#define __VTX_BASE_REPRESENTABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "model/representation/representation.hpp"
#include <set>

namespace VTX
{
	namespace Generic
	{
		class BaseRepresentable
		{
		  public:
			inline const std::set<const Model::Representation::BaseRepresentation *> & getRepresentations() const { return _representations; }

			void addRepresentation( const Model::Representation::BaseRepresentation * p_representation ) { _representations.emplace( p_representation ); };
			void removeRepresentation( const Model::Representation::BaseRepresentation * p_representation ) { _representations.erase( p_representation ); };

		  protected:
			std::set<const Model::Representation::BaseRepresentation *> _representations = std::set<const Model::Representation::BaseRepresentation *>();
		};
	} // namespace Generic
} // namespace VTX
#endif
