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
			void InstantiatedRepresentation::setColorMode( const Generic::COLOR_MODE & p_colorMode )
			{
				Generic::COLOR_MODE * overridedColorMode = new Generic::COLOR_MODE( p_colorMode );
				_data.emplace_back( overridedColorMode );

				_colorMode = overridedColorMode;

				std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
				for ( Generic::BaseRepresentable * target : VTX::Representation::RepresentationManager::get().getTargets( this ) )
					molecules.emplace( target->getMolecule() );

				for ( Model::Molecule * const molecule : molecules )
					molecule->computeColorBuffer();
			}

			void InstantiatedRepresentation::setColor( const Color::Rgb & p_color )
			{
				Color::Rgb * overridedColor = new Color::Rgb( p_color );
				_data.emplace_back( overridedColor );

				_color = overridedColor;

				std::unordered_set<Model::Molecule *> molecules = std::unordered_set<Model::Molecule *>();
				for ( Generic::BaseRepresentable * target : VTX::Representation::RepresentationManager::get().getTargets( this ) )
					molecules.emplace( target->getMolecule() );

				for ( Model::Molecule * const molecule : molecules )
					molecule->computeColorBuffer();
			}

		} // namespace Representation
	}	  // namespace Model
} // namespace VTX
