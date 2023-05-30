#ifndef __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_CORE_STRUCT_RESIDUE_DATA_SET__
#define __VTX_TOOL_ANALYSIS_STRUCTURAL_ALIGNMENT_CORE_STRUCT_RESIDUE_DATA_SET__

#include <app/component/chemistry/residue.hpp>
#include <core/chemdb/residue.hpp>
#include <map>
#include <string>
#include <vector>

namespace VTX::Tool::Analysis::StructuralAlignment::Core::Struct
{
	template<typename T>
	class ResidueDataSet
	{
	  public:
		ResidueDataSet()
		{
			_standardResidueData = std::vector<T>();

			const int standardResidueCount = int( VTX::Core::ChemDB::Residue::SYMBOL::COUNT );
			_standardResidueData.resize( standardResidueCount );
			_standardResidueDataRegistered.resize( standardResidueCount, false );

			_nonStandardResidueData = std::map<std::string, T>();
		}

		const T & get( const App::Component::Chemistry::Residue & p_residue )
		{
			if ( p_residue.isStandardResidue() )
			{
				const int symbolIndex = int( p_residue.getSymbol() );

				if ( !_standardResidueDataRegistered[ symbolIndex ] )
					registerStandardResidueData( p_residue );

				return _standardResidueData[ symbolIndex ];
			}
			else
			{
				const std::string symbolName = p_residue.getSymbolName();

				if ( _nonStandardResidueData.find( symbolName ) == _nonStandardResidueData.end() )
					registerNonStandardResidueData( p_residue );

				return _nonStandardResidueData[ symbolName ];
			}
		}

	  protected:
		void registerStandardResidueData( const App::Component::Chemistry::Residue & p_residue )
		{
			const int symbolIndex = int( p_residue.getSymbol() );
			const T	  data		  = generateResidueData( p_residue );

			_standardResidueData[ symbolIndex ]			  = data;
			_standardResidueDataRegistered[ symbolIndex ] = true;
		}

		void registerNonStandardResidueData( const App::Component::Chemistry::Residue & p_residue )
		{
			const std::string symbolName = p_residue.getSymbolName();
			const T			  data		 = generateResidueData( p_residue );

			_nonStandardResidueData[ symbolName ] = data;
		}

		virtual T generateResidueData( const App::Component::Chemistry::Residue & p_residue ) const = 0;

	  private:
		std::vector<T>			 _standardResidueData;
		std::vector<bool>		 _standardResidueDataRegistered;
		std::map<std::string, T> _nonStandardResidueData;
	};

} // namespace VTX::Tool::Analysis::StructuralAlignment::Core::Struct

#endif
