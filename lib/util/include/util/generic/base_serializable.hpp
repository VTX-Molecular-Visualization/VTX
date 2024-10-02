#ifndef __VTX_APP_CORE_SERIALIZATION_BASE_SERIALIZABLE__
#define __VTX_APP_CORE_SERIALIZATION_BASE_SERIALIZABLE__

#include <util/json/object.hpp>

namespace VTX::Util::Generic
{

	class BaseSerializable
	{
	  public:
		virtual ~BaseSerializable() = default;

		virtual Util::JSon::Object serialize() const								= 0;
		virtual void			   deserialize( const Util::JSon::Object & p_json ) = 0;
	};
} // namespace VTX::Util::Generic

#endif
