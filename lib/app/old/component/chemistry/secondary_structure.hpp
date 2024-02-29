#ifndef __VTX_APP_OLD_COMPONENT_CHEMISTRY_SECONDARY_STRUCTURE__
#define __VTX_APP_OLD_COMPONENT_CHEMISTRY_SECONDARY_STRUCTURE__

#include "_fwd.hpp"
#include "app/old/application/selection/selection.hpp"
#include "app/old/component/generic/base_colorable.hpp"
#include "app/old/core/model/base_model_3d.hpp"
#include "app/old/render/buffer/secondary_structure.hpp"
#include <map>
#include <string>
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Component::Chemistry
{
	class SecondaryStructure : public App::Old::Core::Model::BaseModel3D<App::Old::Render::Buffer::SecondaryStructure>
	{
		VTX_MODEL

	  private:
		using SSDataMap	 = std::map<uint, std::vector<uint>>; // Chain to residues.
		using SSDataPair = std::pair<const uint, std::vector<uint>>;

	  public:
		inline Chemistry::Molecule * const getMolecule() { return _molecule; }

		void refresh();
		void refreshColors();
		void refreshVisibilities();
		void refreshSelection( const App::Old::Application::Selection::SelectionModel::MapChainIds * const p_selection
							   = nullptr );

		inline const std::vector<uint> &	getIndices() const { return _bufferIndices; }
		inline const std::map<uint, uint> & getResidueToControlPointIndice() const { return _residueToIndices; }
		inline std::map<uint, uint> &		getResidueToControlPointIndice() { return _residueToIndices; }

		void print() const;

		const App::Old::Internal::Math::Transform &	 getTransform() const override;
		App::Old::Component::Object3D::Helper::AABB & getAABB() const override;

	  protected:
		void _init() override;
		void _computeAABB() const override;
		void _instantiate3DViews() override;

	  private:
		Chemistry::Molecule * const _molecule;
		SSDataMap					_data = SSDataMap();

		// Carbon alpha (Ca) positions.
		// Add an extra float increasing along the backbone (to determine direction for two sided ss).
		std::vector<Vec4f> _bufferCaPositions = std::vector<Vec4f>();
		// Ca -> O directions.
		std::vector<Vec3f> _bufferCaODirections = std::vector<Vec3f>();
		// Secondary structure types.
		std::vector<uint>			   _bufferSSTypes	   = std::vector<uint>();
		std::vector<Util::Color::Rgba> _bufferColors	   = std::vector<Util::Color::Rgba>();
		std::vector<uint>			   _bufferVisibilities = std::vector<uint>();
		std::vector<uint>			   _bufferSelections   = std::vector<uint>();
		std::vector<uint>			   _bufferIds		   = std::vector<App::Old::Core::Model::ID>();
		std::vector<uint>			   _bufferIndices	   = std::vector<uint>();

		std::map<uint, uint> _residueToIndices	 = std::map<uint, uint>();
		std::map<uint, uint> _residueToPositions = std::map<uint, uint>();

		SecondaryStructure( Molecule * const );
		~SecondaryStructure() = default;

		void _checkOrientationAndFlip( std::vector<Vec3f> & p_directions );
		void _tryConstruct( const uint								  p_chainIdx,
							const std::vector<uint> &				  p_residueIndex,
							const std::vector<Vec4f> &				  p_caPositions,
							std::vector<Vec3f> &					  p_caODirections,
							const std::vector<uint> &				  p_ssTypes,
							const std::vector<Util::Color::Rgba> &	  p_colors,
							const std::vector<uint> &				  p_visibilities,
							const std::vector<App::Old::Core::Model::ID> & p_ids );
	};

} // namespace VTX::App::Old::Component::Chemistry
#endif