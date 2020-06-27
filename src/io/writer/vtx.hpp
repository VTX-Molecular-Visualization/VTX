#ifndef __VTX_WRITER_VTX__
#define __VTX_WRITER_VTX__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_writer_json.hpp"
#include "object3d/scene.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Writer
		{
			class VTX : public BaseWriterJson<Object3D::Scene>
			{
			  public:
				virtual ~VTX() = default;

				virtual void writeFile( const Path & p_path, const Object3D::Scene & p_data ) override;
			};
		} // namespace Writer
	}	  // namespace IO
} // namespace VTX
#endif
