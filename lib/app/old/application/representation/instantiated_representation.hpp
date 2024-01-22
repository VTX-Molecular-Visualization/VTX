#ifndef __VTX_COMPONENT_REPRESENTATION_INSTANTIATED_REPRESENTATION__
#define __VTX_COMPONENT_REPRESENTATION_INSTANTIATED_REPRESENTATION__

#include "app/old/application/generic/base_objectoverride.hpp"
#include "app/old/application/representation/_fwd.hpp"
#include "app/old/application/representation/enum_representation.hpp"
#include "app/old/application/representation/representation_data.hpp"
#include "app/old/application/representation/representation_preset.hpp"
#include "app/old/component/chemistry/_fwd.hpp"
#include "app/old/component/chemistry/secondary_structure.hpp"
#include "app/old/core/event/base_event_receiver_vtx.hpp"
#include "app/old/core/event/vtx_event.hpp"
#include "app/old/core/model/base_model.hpp"
#include "app/old/id.hpp"
#include <string>
#include <util/color/rgba.hpp>

namespace VTX::App::Old::Application::Representation
{
	class InstantiatedRepresentation : public Core::Model::BaseModel, public Core::Event::BaseEventReceiverVTX
	{
		VTX_MODEL

	  public:
		virtual void receiveEvent( const App::Old::Core::Event::VTXEvent & p_event );

		const BaseRepresentable * const getConstTarget() const;
		BaseRepresentable * const		getTarget() const;
		BaseRepresentable * const		getTarget();

		void setTarget( BaseRepresentable * p_target );

		void setLinkedRepresentation( const RepresentationPreset * const p_linkedRepresentation,
									  const bool						 p_eraseOverride = true );

		const std::string & getName() const { return _linkedRepresentation->getName(); };

		const Util::Color::Rgba &						 getColor() const { return _color.getValue(); }
		void											 setColor( const Util::Color::Rgba & p_color,
																   const bool				 p_recomputeBuffers = true,
																   const bool				 p_notify = true );
		const VTX::Core::ChemDB::Color::COLOR_MODE & getColorMode() const { return _colorMode.getValue(); }
		void setColorMode( const VTX::Core::ChemDB::Color::COLOR_MODE & p_colorMode,
						   const bool										p_recomputeBuffers = true,
						   const bool										p_notify		   = true );

		TARGETED_DATA_FLAG getFlagDataTargeted() const
		{
			return RepresentationData::getFlagDataTargeted( _linkedRepresentation->getRepresentationType() );
		};

		bool hasToDrawSphere() const { return _linkedRepresentation->getData().hasToDrawSphere(); };
		const Primitive::Sphere & getSphereData() const { return _sphereData.getValue(); };
		void					  setSphereRadius( const float p_radius, const bool p_notify = true );

		bool hasToDrawCylinder() const { return _linkedRepresentation->getData().hasToDrawCylinder(); };
		const Primitive::Cylinder & getCylinderData() const { return _cylinderData.getValue(); };
		void						setCylinderRadius( const float p_radius, const bool p_notify = true );
		void						setCylinderColorBlendingMode(
								   const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE & p_colorBlendingMode,
								   const bool												 p_notify = true );

		bool hasToDrawRibbon() const { return _linkedRepresentation->getData().hasToDrawRibbon(); };
		const Primitive::Ribbon & getRibbonData() const { return _ribbonData.getValue(); };
		void setRibbonColorMode( const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode,
								 const bool															  p_notify = true );
		void setRibbonColorBlendingMode( const VTX::Core::ChemDB::Color::COLOR_BLENDING_MODE & p_colorBlendingMode,
										 const bool												   p_notify = true );

		bool				   hasToDrawSES() const { return _linkedRepresentation->getData().hasToDrawSES(); };
		const Primitive::SES & getSESData() const { return _sesData.getValue(); };
		void				   setSESResolution( const float, const bool p_notify = true );

		void applyData( const InstantiatedRepresentation & p_source,
						const MEMBER_FLAG &				   p_flag,
						const bool						   p_recomputeBuffers = true,
						const bool						   p_notify			  = true );

		const REPRESENTATION_ENUM & getRepresentationType() const
		{
			return _linkedRepresentation->getRepresentationType();
		}
		const RepresentationPreset * const getLinkedRepresentation() const { return _linkedRepresentation; }

		void		copy( const InstantiatedRepresentation & p_source );
		MEMBER_FLAG getOverridedMembersFlag() const;
		bool		isMemberOverrided( const MEMBER_FLAG & p_member ) const;
		void		refreshSourceValues();
		void		onLinkedRepresentationChange( const App::Old::Core::Event::VTXEvent * const p_event );

	  protected:
		InstantiatedRepresentation( const RepresentationPreset * const p_linkedRepresentation );
		InstantiatedRepresentation();
		~InstantiatedRepresentation();

		const RepresentationPreset * _linkedRepresentation = nullptr;

		BaseRepresentable * _target = nullptr;

		Generic::OverridableParameter<Util::Color::Rgba>						_color;
		Generic::OverridableParameter<VTX::Core::ChemDB::Color::COLOR_MODE> _colorMode;

		Generic::OverridableParameter<Primitive::Sphere>   _sphereData;
		Generic::OverridableParameter<Primitive::Cylinder> _cylinderData;
		Generic::OverridableParameter<Primitive::Ribbon>   _ribbonData;
		Generic::OverridableParameter<Primitive::SES>	   _sesData;

		virtual void _onDataChange() {}

		void _updateTarget( const MOLECULE_COMPUTATION_FLAG & p_flag );
	};

} // namespace VTX::App::Old::Application::Representation
#endif