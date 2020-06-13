#ifndef __VTX_RENDERER_OPTIX_RAY_TRACER_SCENE__
#define __VTX_RENDERER_OPTIX_RAY_TRACER_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#ifdef OPTIX_DEFINED

#include "cuda_buffer.hpp"
#include "model/molecule.hpp"
#include "sphere.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		namespace Optix
		{
			class Scene
			{
			  public:
				Scene() = default;
				~Scene();

				const std::vector<Sphere> & getSpheres() const { return _spheres; }
				const CudaBuffer &			getSpheresDevBuffer() { return _spheresDevBuffer; }
				const CudaBuffer &			getColorsDevBuffer() const { return _colorsDevBuffer; }

				void add( const Model::Molecule * p_molecule );

			  private:
				std::vector<Sphere> _spheres;
				CudaBuffer			_spheresDevBuffer;

				CudaBuffer _colorsDevBuffer;
			};
		} // namespace Optix
	}	  // namespace Renderer
} // namespace VTX

#endif
#endif
