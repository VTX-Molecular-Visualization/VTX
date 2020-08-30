#ifndef __VTX_MODEL_SECONDARY_STRUCTURE__
#define __VTX_MODEL_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model_3d.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace Model
	{
		class SecondaryStructure : public BaseModel3D
		{
		  public:
			SecondaryStructure( Molecule * const );
			~SecondaryStructure();

			inline Model::Molecule * const getMolecule() { return _molecule; }

			void init();
			void bindBuffers() override;
			void unbindBuffers() override;

			const std::vector<uint> & getIndices() const { return _indices; }

			void print() const;

		  private:
			enum ATTRIBUTE_LOCATION
			{
				CONTROL_POINT_POSITION			  = 0,
				CONTROL_POINT_DIRECTION			  = 1,
				CONTROL_POINT_SECONDARY_STRUCTURE = 2,
				CONTROL_POINT_VISIBILITY		  = 3,
			};

			Model::Molecule * const _molecule;

			std::vector<Vec3f> _controlPointPositions			= std::vector<Vec3f>();
			std::vector<Vec3f> _controlPointDirections			= std::vector<Vec3f>();
			std::vector<uint>  _controlPointSecondaryStructures = std::vector<uint>();
			std::vector<uint>  _controlPointVisibilities		= std::vector<uint>();
			std::vector<uint>  _indices							= std::vector<uint>();

			GLuint _vboPositions		   = GL_INVALID_VALUE;
			GLuint _vboDirections		   = GL_INVALID_VALUE;
			GLuint _vboSecondaryStructures = GL_INVALID_VALUE;
			GLuint _vboVisibilities		   = GL_INVALID_VALUE;
			GLuint _ibo					   = GL_INVALID_VALUE;
			GLuint _vao					   = GL_INVALID_VALUE;
		};

	} // namespace Model
} // namespace VTX
#endif
