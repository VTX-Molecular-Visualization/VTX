#ifndef __VTX_APP_ACTION_REPRESENTATION__
#define __VTX_APP_ACTION_REPRESENTATION__

#include "app/action.hpp"
#include "app/core/action/base_action.hpp"
#include "app/application/representation/representation_preset.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include <string>
#include <unordered_set>

namespace VTX::App::Action::Representation
{
	class ReloadPresets : public App::Core::Action::BaseAction
	{
	  public:
		ReloadPresets() {};
		virtual void execute() override;
	};

	class ResetPresetsToDefault : public App::Core::Action::BaseAction
	{
	  public:
		ResetPresetsToDefault() {};
		virtual void execute() override;
	};

	class SavePreset : public App::Core::Action::BaseAction
	{
	  public:
		SavePreset( const App::Application::Representation::RepresentationPreset & p_representation )
		{
			_representations.emplace( &p_representation );
		};
		SavePreset( const std::unordered_set<const App::Application::Representation::RepresentationPreset *> & p_representations )
		{
			for ( const App::Application::Representation::RepresentationPreset * const representation : p_representations )
				_representations.emplace( representation );
		};
		SavePreset( App::Application::Representation::RepresentationLibrary & p_library )
		{
			for ( int i = 0; i < p_library.getRepresentationCount(); i++ )
				_representations.emplace( p_library.getRepresentation( i ) );
			_clearDirectory = true;
		};

		void setAsync( const bool p_async ) { _async = p_async; }

		virtual void execute() override;

	  private:
		std::unordered_set<const App::Application::Representation::RepresentationPreset *> _representations
			= std::unordered_set<const App::Application::Representation::RepresentationPreset *>();

		bool _clearDirectory = false;
		bool _async			 = true;
	};

	class ChangeName : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeName( App::Application::Representation::RepresentationPreset * const p_representation,
							 const std::string &						   p_name ) :
			_representation( p_representation ),
			_name( App::Application::Representation::RepresentationLibrary::get().getValidName( p_name ) )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const std::string							  _name;
	};
	class ChangeQuickAccess : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeQuickAccess( App::Application::Representation::RepresentationPreset * const p_representation,
									const bool									  p_quickAccess ) :
			_representation( p_representation ),
			_quickAccess( p_quickAccess )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const bool									  _quickAccess;
	};

	class ChangeRepresentation : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentation( App::Application::Representation::RepresentationPreset * const p_representation,
									   const App::Application::Representation::REPRESENTATION_ENUM &				 p_representationType ) :
			_representation( p_representation ),
			_representationType( p_representationType )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const App::Application::Representation::REPRESENTATION_ENUM				  _representationType;
	};
	class ChangeColorMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColorMode( App::Application::Representation::RepresentationPreset * const p_representation,
								  const Generic::COLOR_MODE &					p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const Generic::COLOR_MODE					  _colorMode;
	};

	class ChangeColor : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Application::Representation::RepresentationPreset * const p_representation,
							  const Color::Rgba &							p_color ) :
			_representation( p_representation ),
			_color( p_color )
		{
		}
		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const Color::Rgba							  _color;
	};

	class ChangeSphereRadius : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSphereRadius( App::Application::Representation::RepresentationPreset * const p_representation,
									 const float								   p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const float									  _radius;
	};

	class ChangeCylinderRadius : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylinderRadius( App::Application::Representation::RepresentationPreset * const p_representation,
									   const float									 p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const float									  _radius;
	};

	class ChangeCylinderColorBendingMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylinderColorBendingMode( App::Application::Representation::RepresentationPreset * const p_representation,
												 const Generic::COLOR_BLENDING_MODE &		   p_colorBendingMode ) :
			_representation( p_representation ),
			_colorBendingMode( p_colorBendingMode )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const Generic::COLOR_BLENDING_MODE			  _colorBendingMode;
	};

	class ChangeRibbonColorMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorMode( App::Application::Representation::RepresentationPreset * const p_representation,
										Generic::SECONDARY_STRUCTURE_COLOR_MODE &	  p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE _colorMode;
	};

	class ChangeRibbonColorBendingMode : public App::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorBendingMode( App::Application::Representation::RepresentationPreset * const p_representation,
											   const Generic::COLOR_BLENDING_MODE &			 p_colorBendingMode ) :
			_representation( p_representation ),
			_colorBendingMode( p_colorBendingMode )
		{
		}

		void execute();

	  private:
		App::Application::Representation::RepresentationPreset * const _representation;
		const Generic::COLOR_BLENDING_MODE			  _colorBendingMode;
	};

	class AddNewPresetInLibrary : public App::Core::Action::BaseAction
	{
	  public:
		explicit AddNewPresetInLibrary( const std::string & p_representationName ) :
			_representationName( p_representationName )
		{
		}

		void execute();

		const std::string _representationName;
	};

	class CopyPresetInLibrary : public App::Core::Action::BaseAction
	{
	  public:
		explicit CopyPresetInLibrary( const int p_representationIndex ) : _representationIndex( p_representationIndex )
		{
		}

		void execute();

	  private:
		const int _representationIndex;
	};

	class DeletePresetInLibrary : public App::Core::Action::BaseAction
	{
	  public:
		explicit DeletePresetInLibrary( const int p_representationIndex ) :
			_representationIndex( p_representationIndex )
		{
		}

		void execute();

	  private:
		const int _representationIndex;
	};

	class SetAsDefaultRepresentation : public App::Core::Action::BaseAction
	{
	  public:
		explicit SetAsDefaultRepresentation( const int p_representationIndex ) :
			_representationIndex( p_representationIndex )
		{
		}

		void execute();

	  private:
		const int _representationIndex;
	};

} // namespace VTX::App::Action::Representation
#endif
