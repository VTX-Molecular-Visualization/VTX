#ifndef __VTX_CORE_STRUCT_SECONDARY_STRUCTURE__
#define __VTX_CORE_STRUCT_SECONDARY_STRUCTURE__

#include "_fwd.hpp"
#include <map>
#include <util/color/rgba.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Old::Struct
{
	class SecondaryStructure
	{
	  private:
		using SSDataMap = std::map<uint, std::vector<uint>>; // Chain to residues.

	  public:
		SecondaryStructure( Molecule * const );
		~SecondaryStructure() = default;

		inline Molecule * const getMolecule() { return _molecule; }

		void refresh();

		inline const std::vector<uint> &	getIndices() const { return _bufferIndices; }
		inline const std::map<uint, uint> & getResidueToControlPointIndice() const { return _residueToIndices; }
		inline std::map<uint, uint> &		getResidueToControlPointIndice() { return _residueToIndices; }

	  private:
		Molecule * const _molecule;
		SSDataMap		 _data = SSDataMap();

		// Carbon alpha (Ca) positions.
		// Add an extra float increasing along the backbone (to determine direction for two sided ss).
		std::vector<Vec4f> _bufferCaPositions = std::vector<Vec4f>();
		// Ca -> O directions.
		std::vector<Vec3f> _bufferCaODirections = std::vector<Vec3f>();
		// Secondary structure types.
		std::vector<uint> _bufferSSTypes = std::vector<uint>();
		// TODO determine if unique index for object (=modelID) is moved in Core or not.
		// std::vector<uint> _bufferIds = std::vector<App::Core::Model::ID>();

		std::vector<uint> _bufferIndices = std::vector<uint>();

		std::map<uint, uint> _residueToIndices	 = std::map<uint, uint>();
		std::map<uint, uint> _residueToPositions = std::map<uint, uint>();
	};

} // namespace VTX::Core::Old::Struct
#endif
