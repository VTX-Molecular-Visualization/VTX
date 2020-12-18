#include "instantiated_representation.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		namespace Representation
		{
			void InstantiatedRepresentation::setPriority( const int p_priority )
			{
				_priority = p_priority;
				_updateTargets( VTX::Representation::MoleculeComputationFlag::ALL );
			};

			void InstantiatedRepresentation::setColorMode( const Generic::COLOR_MODE & p_colorMode )
			{
				_colorMode = overrideParameter<Generic::COLOR_MODE>( p_colorMode );
				_updateTargets( VTX::Representation::MoleculeComputationFlag::ColorBuffer );
			}

			void InstantiatedRepresentation::setColor( const Color::Rgb & p_color )
			{
				_color = overrideParameter<Color::Rgb>( p_color );
				_updateTargets( VTX::Representation::MoleculeComputationFlag::ColorBuffer );
			}

			void InstantiatedRepresentation::_updateTargets( const VTX::Representation::MoleculeComputationFlag & p_flag ) const
			{
				std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
				for ( Generic::BaseRepresentable * target : VTX::Representation::RepresentationManager::get().getTargets( this ) )
					molecules.emplace( target->getMolecule() );

				for ( Model::Molecule * const molecule : molecules )
				{
					if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::Targets ) != 0 )
						molecule->computeRepresentationTargets();

					if ( int( p_flag & VTX::Representation::MoleculeComputationFlag::ColorBuffer ) != 0 )
						molecule->computeColorBuffer();
				}
			}

		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
