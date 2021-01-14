#ifndef __VTX_MODEL_SECONDARY_STRUCTURE__
#define __VTX_MODEL_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model_3d.hpp"
#include "buffer/secondary_structure.hpp"
#include "color/rgb.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class SecondaryStructure : public BaseModel3D<Buffer::SecondaryStructure>
		{
			VTX_MODEL

		  public:
			enum class VALUE : int
			{
				HELIX_ALPHA_RIGHT = 0,
				HELIX_ALPHA_LEFT  = 1,
				HELIX_3_10_RIGHT  = 2,
				HELIX_3_10_LEFT	  = 3,
				HELIX_PI		  = 4,
				STRAND			  = 5,
				TURN			  = 6,
				COIL			  = 7,
				COUNT
			};
			enum class COLOR_MODE : int
			{
				JMOL,
				PROTEIN,
				CHAIN,
				RESIDUE

			};
			enum class ALGO : int
			{
				STRIDE
			};

			static const Color::Rgb COLORS_JMOL[ uint( VALUE::COUNT ) ];

			inline Model::Molecule * const getMolecule() { return _molecule; }
			inline const COLOR_MODE		   getColorMode() const { return _colorMode; }
			inline void					   setColorMode( const COLOR_MODE p_colorMode )
			{
				_colorMode = p_colorMode;
				refreshColors();
			}
			inline void refreshColors() { _fillBufferColors(); }

			void setCurrentFrame();

			const std::vector<uint> &	 getIndices() const { return _buffferIndices; }
			const std::map<uint, uint> & getResidueToControlPointIndice() const { return _residueToIndices; }
			inline void refreshSelection( const std::map<uint, std::map<uint, std::vector<uint>>> * const p_selection = nullptr ) { _fillBufferSelections( p_selection ); }

			void print() const;

		  protected:
			void _init() override;
			void _fillBuffer() override;
			void _computeAABB() override;
			void _instanciate3DViews() override;

		  private:
			Model::Molecule * const _molecule;
			COLOR_MODE				_colorMode = COLOR_MODE::JMOL;

			std::vector<Vec3f>		_bufferPositions		   = std::vector<Vec3f>();
			std::vector<Vec3f>		_bufferDirections		   = std::vector<Vec3f>();
			std::vector<Vec3f>		_bufferNormals			   = std::vector<Vec3f>();
			std::vector<ushort>		_bufferSecondaryStructures = std::vector<ushort>();
			std::vector<Color::Rgb> _bufferColors			   = std::vector<Color::Rgb>();
			std::vector<ushort>		_bufferSelections		   = std::vector<ushort>();
			std::vector<uint>		_buffferIndices			   = std::vector<uint>();

			std::map<uint, uint> _residueToIndices	 = std::map<uint, uint>();
			std::map<uint, uint> _residueToPositions = std::map<uint, uint>();

			SecondaryStructure( Molecule * const );
			~SecondaryStructure() = default;

			void _fillBufferColors();
			void _fillBufferSelections( const std::map<uint, std::map<uint, std::vector<uint>>> * const = nullptr );
			void _flipTest( Vec3f &, Vec3f & ) const;
		};

	} // namespace Model
} // namespace VTX
#endif
