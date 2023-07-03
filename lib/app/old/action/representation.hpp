#ifndef __VTX_APP_OLD_ACTION_REPRESENTATION__
#define __VTX_APP_OLD_ACTION_REPRESENTATION__

#include "app/old/action.hpp"
#include "app/old/application/representation/representation_library.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/core/action/base_action.hpp"
#include <core/chemdb/color.hpp>
#include <string>
#include <unordered_set>

namespace VTX::App::Old::Action::Representation
{
	class ReloadPresets : public App::Old::Core::Action::BaseAction
	{
	  public:
		ReloadPresets() {};
		virtual void execute() override;
	};

	class ResetPresetsToDefault : public App::Old::Core::Action::BaseAction
	{
	  public:
		ResetPresetsToDefault() {};
		virtual void execute() override;
	};

	class SavePreset : public App::Old::Core::Action::BaseAction
	{
	  public:
		SavePreset( const App::Old::Application::Representation::RepresentationPreset & p_representation )
		{
			_representations.emplace( &p_representation );
		};
		SavePreset( const std::unordered_set<const App::Old::Application::Representation::RepresentationPreset *> &
						p_representations )
		{
			for ( const App::Old::Application::Representation::RepresentationPreset * const representation :
				  p_representations )
				_representations.emplace( representation );
		};
		SavePreset( App::Old::Application::Representation::RepresentationLibrary & p_library )
		{
			for ( int i = 0; i < p_library.getRepresentationCount(); i++ )
				_representations.emplace( p_library.getRepresentation( i ) );
			_clearDirectory = true;
		};

		void setAsync( const bool p_async ) { _async = p_async; }

		virtual void execute() override;

	  private:
		std::unordered_set<const App::Old::Application::Representation::RepresentationPreset *> _representations
			= std::unordered_set<const App::Old::Application::Representation::RepresentationPreset *>();

		bool _clearDirectory = false;
		bool _async			 = true;
	};

	class ChangeName : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeName( App::Old::Application::Representation::RepresentationPreset * const p_representation,
							 const std::string &											p_name ) :
			_representation( p_representation ),
			_name( App::Old::Application::Representation::RepresentationLibrary::get().getValidName( p_name ) )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const std::string											   _name;
	};
	class ChangeQuickAccess : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeQuickAccess( App::Old::Application::Representation::RepresentationPreset * const p_representation,
									const bool													   p_quickAccess ) :
			_representation( p_representation ),
			_quickAccess( p_quickAccess )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const bool													   _quickAccess;
	};

	class ChangeRepresentation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRepresentation(
			App::Old::Application::Representation::RepresentationPreset * const p_representation,
			const App::Old::Application::Representation::REPRESENTATION_ENUM &  p_representationType ) :
			_representation( p_representation ),
			_representationType( p_representationType )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const App::Old::Application::Representation::REPRESENTATION_ENUM	   _representationType;
	};
	class ChangeColorMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColorMode( App::Old::Application::Representation::RepresentationPreset * const p_representation,
								  const VTX::Core::ChemDB::Color::COLOR_MODE &					 p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const VTX::Core::ChemDB::Color::COLOR_MODE					   _colorMode;
	};

	class ChangeColor : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeColor( App::Old::Application::Representation::RepresentationPreset * const p_representation,
							  const Util::Color::Rgba &										 p_color ) :
			_representation( p_representation ),
			_color( p_color )
		{
		}
		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const Util::Color::Rgba										   _color;
	};

	class ChangeSphereRadius : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeSphereRadius( App::Old::Application::Representation::RepresentationPreset * const p_representation,
									 const float													p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const float													   _radius;
	};

	class ChangeCylinderRadius : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylinderRadius( App::Old::Application::Representation::RepresentationPreset * const p_representation,
									   const float													  p_radius ) :
			_representation( p_representation ),
			_radius( p_radius )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const float													   _radius;
	};

	class ChangeCylinderColorBendingMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeCylinderColorBendingMode(
			App::Old::Application::Representation::RepresentationPreset * const p_representation,
			const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE &		   p_colorBendingMode ) :
			_representation( p_representation ),
			_colorBendingMode( p_colorBendingMode )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE			   _colorBendingMode;
	};

	class ChangeRibbonColorMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorMode( App::Old::Application::Representation::RepresentationPreset * const p_representation,
										VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE &	   p_colorMode ) :
			_representation( p_representation ),
			_colorMode( p_colorMode )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE _colorMode;
	};

	class ChangeRibbonColorBendingMode : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ChangeRibbonColorBendingMode(
			App::Old::Application::Representation::RepresentationPreset * const p_representation,
			const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE &		   p_colorBendingMode ) :
			_representation( p_representation ),
			_colorBendingMode( p_colorBendingMode )
		{
		}

		void execute();

	  private:
		App::Old::Application::Representation::RepresentationPreset * const _representation;
		const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE			   _colorBendingMode;
	};

	class AddNewPresetInLibrary : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit AddNewPresetInLibrary( const std::string & p_representationName ) :
			_representationName( p_representationName )
		{
		}

		void execute();

		const std::string _representationName;
	};

	class CopyPresetInLibrary : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit CopyPresetInLibrary( const int p_representationIndex ) : _representationIndex( p_representationIndex )
		{
		}

		void execute();

	  private:
		const int _representationIndex;
	};

	class DeletePresetInLibrary : public App::Old::Core::Action::BaseAction
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

	class SetAsDefaultRepresentation : public App::Old::Core::Action::BaseAction
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

} // namespace VTX::App::Old::Action::Representation
#endif
