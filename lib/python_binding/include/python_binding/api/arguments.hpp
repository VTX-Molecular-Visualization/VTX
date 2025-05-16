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
	/**
	 * @brief Class wrapping python keyword arguments
	 */
	class PythonKwargs
	{
	  public:
		PythonKwargs( const pybind11::kwargs & );

		/**
		 * @brief Returns the number of arguments
		 */
		size_t size() const;

		/**
		 * @brief Returns whether the provided key is contained within the kwargs
		 */
		bool contains( const char * ) const;

		/**
		 * @brief Fills the last argument with the content of the kwargs
		 * @param p_key Name of the python argument
		 * @param p_value Output value
		 */
		void get( const char * p_key, std::string & p_value ) const;
		void get( const char * p_key, std::vector<std::string> & p_value ) const;
		void get( const char * p_key, std::vector<size_t> & p_value ) const;
		void get( const char * p_key, std::vector<uint32_t> & p_value ) const;

		/**
		 * @brief Helper template to fill a vector of type converted from a string.
		 * @tparam T Type to be converted to
		 * @param p_key Name of the kwarg
		 * @param p_conversionFunction Pointer for the conversion function
		 * @param p_value Output value
		 */
		template<typename T>
		void get(
			const char * p_key,
			T ( *p_conversionFunction )( const std::string & ),
			std::vector<T> & p_value
		) const
		{
			std::vector<std::string> strValues;
			get( p_key, strValues );
			p_value.clear();
			p_value.reserve( strValues.size() );
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
