#ifndef __VTX_RENDERER_OPTIX_RAY_TRACER_SCENE__
#define __VTX_RENDERER_OPTIX_RAY_TRACER_SCENE__

#ifdef _MSC_VER
#pragma once
#endif

#ifdef OPTIX_DEFINED

#include "cuda/buffer.hpp"
#include "cylinder.hpp"
#include "model/molecule.hpp"
#include "sphere.hpp"
#include <vector>

namespace VTX::Renderer::Optix
{
	class Scene
	{
	  public:
		Scene() = default;
		~Scene();

		const std::vector<Sphere> &	  getSpheres() const { return _spheres; }
		const std::vector<Cylinder> & getCylinders() const { return _cylinders; }

		const CUdeviceptr getSpheresDevPtr() const { return _spheresDevBuffer.getDevicePtr(); }
		const CUdeviceptr getCylindersDevPtr() const { return _cylindersDevBuffer.getDevicePtr(); }
		const CUdeviceptr getColorsDevPtr() const { return _colorsDevBuffer.getDevicePtr(); }

		void add( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation );

	  private:
		void _addSpheres( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation );
		void _addCylinders( const Model::Molecule * p_molecule, const Generic::REPRESENTATION p_representation );

	  private:
		std::vector<Sphere>	  _spheres;
		CUDA::Buffer		  _spheresDevBuffer;
		std::vector<Cylinder> _cylinders;
		CUDA::Buffer		  _cylindersDevBuffer;

		std::vector<float3> _colors;
		CUDA::Buffer		_colorsDevBuffer;
	};
} // namespace VTX::Renderer::Optix

#endif
#endif
