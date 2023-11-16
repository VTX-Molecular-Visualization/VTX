#include "python_binding/wrapper/object.hpp"

namespace VTX::PythonBinding::Wrapper
{
	Object::~Object() = default;
	std::string Object::getObjectName() const
	{
		return _pyObject.attr( "__class__" ).attr( "__name__" ).cast<std::string>();
	}

} // namespace VTX::PythonBinding::Wrapper
