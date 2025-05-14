#ifndef __VTX_PYTHON_API_ARGUMENTS__
#define __VTX_PYTHON_API_ARGUMENTS__

#include <memory>
#include <string>
#include <vector>

namespace pybind11
{
	class kwargs;
}

namespace VTX::PythonBinding::API
{
	class PythonKwargs
	{
	  public:
		size_t size() const;

		bool contains( const char * ) const;

		void get( const char * p_key, std::string & p_value ) const;
		void get( const char * p_key, std::vector<std::string> & p_value ) const;
		void get( const char * p_key, std::vector<size_t> & p_value ) const;
		void get( const char * p_key, std::vector<uint32_t> & p_value ) const;
		template<typename T>
		void get(
			const char * p_key,
			T ( *p_conversionFunction )( const std::string & ),
			std::vector<T> & p_value
		) const
		{
			std::vector<std::string> strValues;
			get( p_key, strValues );
			for ( auto & strValue : strValues )
				p_value.push_back( ( *p_conversionFunction )( strValue ) );
		}

	  private:
		struct Del
		{
			void operator()( pybind11::kwargs * ) noexcept;
		};
		std::unique_ptr<pybind11::kwargs, Del> _ptr = nullptr;
	};

} // namespace VTX::PythonBinding::API
#endif
