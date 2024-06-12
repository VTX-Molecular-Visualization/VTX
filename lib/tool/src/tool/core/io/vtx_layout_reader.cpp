#include "tool/core/io/vtx_layout_reader.hpp"
#include "tool/core/serialization/ui_core_serializers.hpp"
#include <app/application/system/serializer.hpp>

namespace VTX::UI::Core::IO
{
	void VTXLayoutReader::read( const FilePath & p_filePath )
	{
		_result = Result();
		App::SERIALIZER().readObject<LayoutDescriptor>( p_filePath, _result.layoutDescriptor );
	}

} // namespace VTX::UI::Core::IO
