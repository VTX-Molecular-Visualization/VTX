#include "util/json/array.hpp"

namespace VTX::Util::JSon
{
	Array::Array()						   = default;
	Array::Array( const Array & p_source ) = default;

	Array::Array( const std::initializer_list<BasicJSon> & p_init ) : _objs( p_init ) {}
	Array::Array( const std::vector<BasicJSon> & p_init ) : _objs( p_init ) {}

	Array::~Array() = default;

	std::vector<BasicJSon>::const_iterator Array::begin() const { return _objs.cbegin(); }
	std::vector<BasicJSon>::const_iterator Array::end() const { return _objs.cend(); }

	BasicJSon & Array::operator[]( const size_t p_index ) { return _objs[ p_index ]; }
	const BasicJSon & Array::operator[]( const size_t p_index ) const { return _objs[ p_index ]; }

	void Array::emplace_back( const BasicJSon & p_obj ) { _objs.emplace_back( p_obj ); }

	size_t Array::size() const { return _objs.size(); }

} // namespace VTX::Util::JSon
