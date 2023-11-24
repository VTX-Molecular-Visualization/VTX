#ifndef __VTX_UTIL_JSON_DOCUMENT__
#define __VTX_UTIL_JSON_DOCUMENT__

#include "_fwd.hpp"
#include "util/json/basic_json.hpp"
#include <memory>
#include <string>

namespace VTX::Util::JSon
{
	template<typename T>
	concept BasicJSonConcept = std::constructible_from<BasicJSon, T>;

	class Document
	{
	  public:
		Document();
		Document( const Document & p_source );

		Document( const BasicJSon & p_init );
		Document( const std::initializer_list<BasicJSon> & p_init );

		template<BasicJSonConcept T>
		Document( const T & p_value ) : Document( BasicJSon( p_value ) )
		{
		}

		~Document();

		Document & operator=( const Document & p_source );

		void		clear();
		std::string toString() const;

	  private:
		class Impl;
		std::unique_ptr<Impl> _impl;
	};

} // namespace VTX::Util::JSon
#endif
