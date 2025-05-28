#ifndef __VTX_PYTHON_BINDING_COMMAND_FILTER__
#define __VTX_PYTHON_BINDING_COMMAND_FILTER__

#include <string>

namespace VTX::PythonBinding
{
	/**
	 * @brief Class responsible for holding filter result
	 */
	class FilterResult
	{
	public:
		FilterResult( bool, std::string );
		FilterResult( bool );

		/**
		 * @brief Returns true if the command is harmful to VTX. It has been done like that to enable the if(auto rslt = filter) syntax.
		 */
		operator bool() const noexcept;
		bool isHarmful() const noexcept;

		/**
		 * @brief Returns why the command is harmful or nothing if the command shouldn't be filtered
		 * @return 
		 */
		const std::string & why() const noexcept;

	private:
		bool _harmful = false;
		std::string _why;
	};
	
	/**
	 * @brief Function responsible for filtering commands that are harmful to VTX
	 * @param  
	 * @return 
	 */
	FilterResult filter( const std::string & )  noexcept;
}

#endif