#include "contour_buildup.hpp"

namespace VTX::Buffer
{
	void ContourBuildup::generate()
	{
		_ssboTorusPatches.create( Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
		_ssboSphericalTrianglePatches.create( Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );
		_ssboProbeIntersection.create( Renderer::GL::BufferStorage::Target::SHADER_STORAGE_BUFFER );

		_vao.create();
	}
} // namespace VTX::Buffer
