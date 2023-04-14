#ifndef __VTX_UI_WIDGET_SEQUENCE_CHAIN_BUILDER_DATA__
#define __VTX_UI_WIDGET_SEQUENCE_CHAIN_BUILDER_DATA__

#include "dataset/sequence_dataset.hpp"
#include <QString>
#include <app/old_app/model/chain.hpp>
#include <vector>

namespace VTX::UI::Widget::Sequence
{
	class ChainSequenceBuilder
	{
	  private:
		enum class DatasetType
		{
			STANDARD,
			UNKNOWN,
			MISSING,

			NOT_SET,
		};
		class BuildingDatasetMetadata
		{
		  public:
			BuildingDatasetMetadata() : BuildingDatasetMetadata( DatasetType::NOT_SET ) {};
			BuildingDatasetMetadata( const DatasetType & p_type ) : type( p_type ) {};

			DatasetType type;

			uint firstResidueIndex;
			uint lastResidueIndex;
		};

	  public:
		ChainSequenceBuilder( const Model::Chain & p_chain );
		~ChainSequenceBuilder();

		void generate();
		void update();
		void clear();

		const std::vector<Dataset::SequenceDisplayDataset *> & getDataSet() const { return _dataset; };
		inline uint getCharCount() const { return _dataset.back()->getLastCharIndex(); };

	  private:
		void _emplaceDataSet( const BuildingDatasetMetadata & p_data );
		void _emplaceResidueDataSet( const BuildingDatasetMetadata & p_data );
		void _emplaceUnknownResidueDataSet( const BuildingDatasetMetadata & p_data );
		void _emplaceMissingResidueDataSet( const BuildingDatasetMetadata & p_data );

		Model::Residue * const _getResidue( const uint p_localResidueIndex ) const;

		uint										   _currentCharPos	   = 0;
		bool										   _lastDataWasUnknown = false;
		std::vector<Dataset::SequenceDisplayDataset *> _dataset = std::vector<Dataset::SequenceDisplayDataset *>();

	  private:
		const Model::Chain & _chain;
	};

} // namespace VTX::UI::Widget::Sequence
#endif
