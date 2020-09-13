#ifndef __VTX_MODEL_SECONDARY_STRUCTURE__
#define __VTX_MODEL_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model_3d.hpp"
#include "color/rgb.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class SecondaryStructure : public BaseModel3D
		{
		  public:
			enum class VALUE : int
			{
				HELIX_ALPHA,
				HELIX_3_10,
				HELIX_PI,
				STRAND,
				TURN,
				COIL,
				COUNT
			};

			enum class ALGO : int
			{
				STRIDE
			};

			static const Color::Rgb SECONDARY_STRUCTURE_COLORS_JMOL[ uint( VALUE::COUNT ) ];

			SecondaryStructure( Molecule * const );
			~SecondaryStructure();

			inline Model::Molecule * const getMolecule() { return _molecule; }

			void init();
			void bindBuffers() override;
			void unbindBuffers() override;

			void setCurrentFrame();

			const std::vector<uint> &	 getIndices() const { return _indices; }
			const std::map<uint, uint> & getResidueToControlPointIndice() const
			{
				return _residueToControlPointIndices;
			}

			void print() const;

		  private:
			enum ATTRIBUTE_LOCATION
			{
				CONTROL_POINT_POSITION			  = 0,
				CONTROL_POINT_DIRECTION			  = 1,
				CONTROL_POINT_SECONDARY_STRUCTURE = 2,
				CONTROL_POINT_COLOR				  = 3,
				CONTROL_POINT_VISIBILITY		  = 4,
			};
			enum class COLOR_MODE : int
			{
				JMOL,
				CHAIN
			};

			Model::Molecule * const _molecule;
			COLOR_MODE				_colorMode = COLOR_MODE::JMOL;

			std::vector<Vec3f>		_controlPointPositions			 = std::vector<Vec3f>();
			std::vector<Vec3f>		_controlPointDirections			 = std::vector<Vec3f>();
			std::vector<uint>		_controlPointSecondaryStructures = std::vector<uint>();
			std::vector<Color::Rgb> _controlPointColors				 = std::vector<Color::Rgb>();
			std::vector<uint>		_controlPointVisibilities		 = std::vector<uint>();
			std::vector<uint>		_indices						 = std::vector<uint>();

			std::map<uint, uint> _residueToControlPointIndices = std::map<uint, uint>();

			GLuint _vboPositions		   = GL_INVALID_VALUE;
			GLuint _vboDirections		   = GL_INVALID_VALUE;
			GLuint _vboSecondaryStructures = GL_INVALID_VALUE;
			GLuint _vboColors			   = GL_INVALID_VALUE;
			GLuint _vboVisibilities		   = GL_INVALID_VALUE;
			GLuint _ibo					   = GL_INVALID_VALUE;
			GLuint _vao					   = GL_INVALID_VALUE;

			void _flipTest( Vec3f &, Vec3f & ) const;
		};

	} // namespace Model
} // namespace VTX
#endif
