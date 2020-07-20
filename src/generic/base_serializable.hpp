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
			void		   fromJson( nlohmann::json & ) {};
			nlohmann::json toJson() const {};
		};
	} // namespace Generic
} // namespace VTX
#endif
