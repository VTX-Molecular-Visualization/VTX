#ifndef __VTX_MODEL_SECONDARY_STRUCTURE__
#define __VTX_MODEL_SECONDARY_STRUCTURE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model_3d.hpp"
#include "buffer/secondary_structure.hpp"
#include "color/rgb.hpp"
#include "generic/base_colorable.hpp"
#include "model/selection.hpp"
#include <string>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Molecule;

		class SecondaryStructure : public BaseModel3D<Buffer::SecondaryStructure>
		{
			VTX_MODEL

		  public:
			enum class TYPE : int
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

			inline Model::Molecule * const getMolecule() { return _molecule; }

			void refresh( const bool p_refreshBuffers = true );
			void refreshColors();
			void refreshVisibilities();

			inline const std::vector<uint> &	getIndices() const { return _bufferIndices; }
			inline const std::map<uint, uint> & getResidueToControlPointIndice() const { return _residueToIndices; }
			inline std::map<uint, uint> &		getResidueToControlPointIndice() { return _residueToIndices; }

			inline void refreshSelection( const Model::Selection::MapChainIds * const p_selection = nullptr )
			{
				_fillBufferSelections( p_selection );
			}

			void print() const;

			const Math::Transform & getTransform() const override;
			const Math::AABB &		getAABB() const override;

		  protected:
			void _init() override;
			void _fillBuffer() override;
			void _computeAABB() const override;
			void _instantiate3DViews() override;

		  private:
			Model::Molecule * const			  _molecule;
			std::map<uint, std::vector<uint>> _data = std::map<uint, std::vector<uint>>(); // Chain to residues.

			// Carbon alpha (Ca) positions.
			// Add an extra float increasing along the backbone (to determine direction for two sided ss).
			std::vector<Vec4f> _bufferCaPositions = std::vector<Vec4f>();
			// Ca -> O directions.
			std::vector<Vec3f> _bufferCaODirections = std::vector<Vec3f>();
			// Secondary structure types.
			std::vector<uint>		_bufferSSTypes		= std::vector<uint>();
			std::vector<Color::Rgb> _bufferColors		= std::vector<Color::Rgb>();
			std::vector<uint>		_bufferVisibilities = std::vector<uint>();
			std::vector<uint>		_bufferSelections	= std::vector<uint>();
			std::vector<uint>		_bufferIndices		= std::vector<uint>();

			std::map<uint, uint> _residueToIndices	 = std::map<uint, uint>();
			std::map<uint, uint> _residueToPositions = std::map<uint, uint>();

			SecondaryStructure( Molecule * const );
			~SecondaryStructure() = default;

			void _fillBufferSelections( const Model::Selection::MapChainIds * const = nullptr );
			void _checkOrientationAndFlip( std::vector<Vec3f> & p_directions );
		};

	} // namespace Model
} // namespace VTX
#endif
