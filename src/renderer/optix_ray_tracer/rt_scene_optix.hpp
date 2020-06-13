#ifndef __VTX_RENDERER_OPTIX_RAY_TRACER_SCENE__
#define __VTX_RENDERER_OPTIX_RAY_TRACER_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#ifdef OPTIX_DEFINED

#include "cuda/buffer.hpp"
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
				const CUDA::Buffer &		getSpheresDevBuffer() { return _spheresDevBuffer; }
				const CUDA::Buffer &		getColorsDevBuffer() const { return _colorsDevBuffer; }

				void add( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation );

			  private:
				void _addSpheres( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation );

			  private:
				std::vector<Sphere> _spheres;
				CUDA::Buffer		_spheresDevBuffer;

				CUDA::Buffer _colorsDevBuffer;
			};
		} // namespace Optix
	}	  // namespace Renderer
} // namespace VTX

#endif
#endif
