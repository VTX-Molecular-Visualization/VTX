#ifndef __VTX_APP_ACTION_INSTANTIATED_REPRESENTATION__
#define __VTX_APP_ACTION_INSTANTIATED_REPRESENTATION__

#include "app/action.hpp"
#include "app/application/representation/instantiated_representation.hpp"
#include "app/core/action/base_action.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include "app/application/representation/base_representable.hpp"
#include <type_traits>
#include <unordered_set>

namespace VTX::App::Action::InstantiatedRepresentation
{
	class ChangeColorMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColorMode( App::Application::Representation::InstantiatedRepresentation * const p_representation,
								  const Generic::COLOR_MODE &								p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		virtual void execute() override;

	  private:
		const Generic::COLOR_MODE								  _colorMode;
		App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Application::Representation::InstantiatedRepresentation * const p_representation,
							  const Util::Color::Rgba &										p_color ) :
			_color( p_color )
		{
			_instantiatedRepresentations.emplace( p_representation );
		}

		template<typename T, typename = std::enable_if<std::is_base_of<App::Application::Representation::BaseRepresentable, T>::value>>
		explicit ChangeColor( const std::unordered_set<T *> & p_representables, const Util::Color::Rgba & p_color ) :
			_color( p_color )
		{
			for ( T * const representable : p_representables )
			{
				if ( !representable->hasCustomRepresentation() )
				{
					App::Application::Representation::InstantiatedRepresentation * instantiatedRepresentation
						= App::Application::Representation::RepresentationManager::get().instantiateCopy(
							representable->getRepresentation(), *representable, true, false );
				}

				_instantiatedRepresentations.emplace( representable->getCustomRepresentation() );
			}
		}

		virtual void execute() override;

	  private:
		const Util::Color::Rgba														_color;
		std::unordered_set<App::Application::Representation::InstantiatedRepresentation *> _instantiatedRepresentations
			= std::unordered_set<App::Application::Representation::InstantiatedRepresentation *>();
	};

	class ChangeSphereRadius : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSphereRadius( App::Application::Representation::InstantiatedRepresentation * const p_representation,
									 const float											   p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		virtual void execute() override;

	  private:
		const float												  _radius;
		App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
	class ChangeCylinderRadius : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylinderRadius( App::Application::Representation::InstantiatedRepresentation * const p_representation,
									   const float												 p_radius ) :
			_instantiatedRepresentation( p_representation ),
			_radius( p_radius )
		{
		}

		virtual void execute() override;

	  private:
		const float												  _radius;
		App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeCylindeColorBlendingMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylindeColorBlendingMode(
			App::Application::Representation::InstantiatedRepresentation * const p_representation,
			const Generic::COLOR_BLENDING_MODE &					  p_mode ) :
			_instantiatedRepresentation( p_representation ),
			_mode( p_mode )
		{
		}

		virtual void execute() override;

	  private:
		const Generic::COLOR_BLENDING_MODE						  _mode;
		App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeRibbonColorMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorMode( App::Application::Representation::InstantiatedRepresentation * const p_representation,
										const Generic::SECONDARY_STRUCTURE_COLOR_MODE &			  p_colorMode ) :
			_instantiatedRepresentation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		virtual void execute() override;

	  private:
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE			  _colorMode;
		App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};

	class ChangeRibbonColorBlendingMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorBlendingMode(
			App::Application::Representation::InstantiatedRepresentation * const p_representation,
			const Generic::COLOR_BLENDING_MODE &					  p_mode ) :
			_instantiatedRepresentation( p_representation ),
			_mode( p_mode )
		{
		}

		virtual void execute() override;

	  private:
		const Generic::COLOR_BLENDING_MODE						  _mode;
		App::Application::Representation::InstantiatedRepresentation * const _instantiatedRepresentation;
	};
} // namespace VTX::App::Action::InstantiatedRepresentation
#endif
