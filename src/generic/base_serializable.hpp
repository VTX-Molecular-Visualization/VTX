#ifndef __VTX_BASE_SERIALIZABLE__
#define __VTX_BASE_SERIALIZABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <nlohmann/json.hpp>

namespace VTX
{
	namespace Generic
	{
		class BaseSerializable
		{
		  public:
			virtual ~BaseSerializable() = default;

			virtual void		   fromJson( nlohmann::json & ) = 0;
			virtual nlohmann::json toJson() const				= 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
