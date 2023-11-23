#include "util/json/array.hpp"
#include "util/enum.hpp"
#include "util/json/basic_json.hpp"
#include "util/logger.hpp"
#include <nlohmann/json.hpp>

namespace VTX::Util::JSon
{
	Array::Array()						   = default;
	Array::Array( const Array & p_source ) = default;

	Array::Array( const BasicJSon & p_singleValue )
	{
		_objs.resize( 1 );
		_objs[ 0 ] = p_singleValue;
	}

	Array::Array( const std::initializer_list<BasicJSon> & p_init ) : _objs( p_init ) {}
	Array::Array( const std::vector<BasicJSon> & p_init ) : _objs( p_init ) {}

	Array::~Array() = default;

	std::vector<BasicJSon>::const_iterator Array::begin() const { return _objs.cbegin(); }
	std::vector<BasicJSon>::const_iterator Array::end() const { return _objs.cend(); }

	void Array::emplace_back( const BasicJSon & p_obj ) { _objs.emplace_back( p_obj ); }

} // namespace VTX::Util::JSon
