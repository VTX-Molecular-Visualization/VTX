#ifndef __VTX_MODEL_SECONDARY_STRUCTURE__
#define __VTX_MODEL_SECONDARY_STRUCTURE__

#include "generic/base_colorable.hpp"
#include "model/base_model_3d.hpp"
#include "model/color/rgb.hpp"
#include "model/selection/selection.hpp"
#include <string>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class SecondaryStructure : public BaseModel3D
		{
			VTX_MODEL

		  private:
			using SSDataMap	 = std::map<uint, std::vector<uint>>; // Chain to residues.
			using SSDataPair = std::pair<const uint, std::vector<uint>>;

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

			void refresh();
			void refreshColors();
			void refreshVisibilities();
			void refreshSelection( const Model::Selection::MapChainIds * const p_selection = nullptr );

			inline const std::vector<uint> &	getIndices() const { return _bufferIndices; }
			inline const std::map<uint, uint> & getResidueToControlPointIndice() const { return _residueToIndices; }
			inline std::map<uint, uint> &		getResidueToControlPointIndice() { return _residueToIndices; }

			void print() const;

			const Math::Transform &	 getTransform() const override;
			Object3D::Helper::AABB & getAABB() const override;

		  protected:
			void _init() override;
			void _computeAABB() const override;

		  private:
			Model::Molecule * const _molecule;
			SSDataMap				_data = SSDataMap();

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
			std::vector<uint>		_bufferIds			= std::vector<Model::ID>();
			std::vector<uint>		_bufferIndices		= std::vector<uint>();

			std::map<uint, uint> _residueToIndices	 = std::map<uint, uint>();
			std::map<uint, uint> _residueToPositions = std::map<uint, uint>();

			SecondaryStructure( Molecule * const );
			~SecondaryStructure() = default;

			void _checkOrientationAndFlip( std::vector<Vec3f> & p_directions );
			void _tryConstruct( const uint						p_chainIdx,
								const std::vector<uint> &		p_residueIndex,
								const std::vector<Vec4f> &		p_caPositions,
								std::vector<Vec3f> &			p_caODirections,
								const std::vector<uint> &		p_ssTypes,
								const std::vector<Color::Rgb> & p_colors,
								const std::vector<uint> &		p_visibilities,
								const std::vector<Model::ID> &	p_ids );
		};

	} // namespace Model
} // namespace VTX
#endif
