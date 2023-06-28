#ifndef __VTX_APP_OLD_ACTION_INSTANTIATED_REPRESENTATION__
#define __VTX_APP_OLD_ACTION_INSTANTIATED_REPRESENTATION__

#include "app/old/action.hpp"
#include "app/old/application/representation/base_representable.hpp"
#include "app/old/application/representation/instantiated_representation.hpp"
#include "app/old/application/representation/representation_manager.hpp"
#include "app/old/core/action/base_action.hpp"
#include <core/chemdb/color.hpp>
#include <type_traits>
#include <unordered_set>

namespace VTX::App::Old::Action::InstantiatedRepresentation
{
	class ChangeColorMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColorMode( App::Old::Application::Representation::InstantiatedRepresentation * const p_representation,
								  const VTX::Core::ChemDB::Color::COLOR_MODE &						   p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::Core::ChemDB::Color::COLOR_MODE							 _colorMode;
		App::Old::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Old::Application::Representation::InstantiatedRepresentation * const p_representation,
							  const Util::Color::Rgba &											   p_color ) :
			_color( p_color )
		{
			_instantiatedRepresentations.emplace( p_representation );
		}

		template<typename T,
				 typename
				 = std::enable_if<std::is_base_of<App::Old::Application::Representation::BaseRepresentable, T>::value>>
		explicit ChangeColor( const std::unordered_set<T *> & p_representables, const Util::Color::Rgba & p_color ) :
			_color( p_color )
		{
			for ( T * const representable : p_representables )
			{
				if ( !representable->hasCustomRepresentation() )
				{
					App::Old::Application::Representation::InstantiatedRepresentation * instantiatedRepresentation
						= App::Old::Application::Representation::RepresentationManager::get().instantiateCopy(
							representable->getRepresentation(), *representable, true, false );
				}

				_instantiatedRepresentations.emplace( representable->getCustomRepresentation() );
			}
		}

		virtual void execute() override;

	  private:
		const Util::Color::Rgba															   _color;
		std::unordered_set<App::Old::Application::Representation::InstantiatedRepresentation *> _instantiatedRepresentations
			= std::unordered_set<App::Old::Application::Representation::InstantiatedRepresentation *>();
	};

	class ChangeSphereRadius : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSphereRadius(
			App::Old::Application::Representation::InstantiatedRepresentation * const p_representation,
			const float															 p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		virtual void execute() override;

	  private:
		const float															 _radius;
		App::Old::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
	class ChangeCylinderRadius : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylinderRadius(
			App::Old::Application::Representation::InstantiatedRepresentation * const p_representation,
			const float															 p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		virtual void execute() override;

	  private:
		const float															 _radius;
		App::Old::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeCylindeColorBlendingMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylindeColorBlendingMode(
			App::Old::Application::Representation::InstantiatedRepresentation * const p_representation,
			const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE &				 p_mode ) :
			_instantiatedRepresentation( p_representation ),
			_mode( p_mode )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE					 _mode;
		App::Old::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeRibbonColorMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorMode(
			App::Old::Application::Representation::InstantiatedRepresentation * const p_representation,
			const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE &	 p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE		 _colorMode;
		App::Old::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeRibbonColorBlendingMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorBlendingMode(
			App::Old::Application::Representation::InstantiatedRepresentation * const p_representation,
			const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE &				 p_mode ) :
			_instantiatedRepresentation( p_representation ),
			_mode( p_mode )
		{
		}

		virtual void execute() override;

	  private:
		const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE					 _mode;
		App::Old::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
} // namespace VTX::App::Old::Action::InstantiatedRepresentation
#endif
