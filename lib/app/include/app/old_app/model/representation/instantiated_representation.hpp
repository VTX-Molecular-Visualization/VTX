#ifndef __VTX_MODEL_INSTANTIATED_REPRESENTATION__
#define __VTX_MODEL_INSTANTIATED_REPRESENTATION__

#include "app/old_app/color/rgba.hpp"
#include "app/old_app/event/base_event_receiver_vtx.hpp"
#include "app/old_app/event/event.hpp"
#include "app/old_app/generic/base_objectoverride.hpp"
#include "app/old_app/id.hpp"
#include "app/old_app/model/base_model.hpp"
#include "app/old_app/model/secondary_structure.hpp"
#include "representation.hpp"
#include "representation_data.hpp"
#include "representation_enum.hpp"

namespace VTX::Model
{
	class Molecule;
	class SecondaryStructure;

} // namespace VTX::Model
namespace VTX::Generic
{
	class BaseRepresentable;
}

namespace VTX::Model::Representation
{
	class InstantiatedRepresentation : public BaseModel, public Generic::BaseObjectOverride, Event::BaseEventReceiverVTX
	{
		VTX_MODEL

	  public:
		virtual void receiveEvent( const Event::VTXEvent & p_event );

		const Generic::BaseRepresentable * const getConstTarget() const;
		Generic::BaseRepresentable * const		 getTarget() const;
		Generic::BaseRepresentable * const		 getTarget();

		void setTarget( Generic::BaseRepresentable * p_target );

		void setLinkedRepresentation( const Representation * const p_linkedRepresentation,
									  const bool				   p_eraseOverride = true );

		const std::string & getName() const { return _linkedRepresentation->getName(); };

		const Color::Rgba & getColor() const { return _color.getValue(); }
		void setColor( const Color::Rgba & p_color, const bool p_recomputeBuffers = true, const bool p_notify = true );
		const Generic::COLOR_MODE & getColorMode() const { return _colorMode.getValue(); }
		void						setColorMode( const Generic::COLOR_MODE & p_colorMode,
												  const bool				  p_recomputeBuffers = true,
												  const bool				  p_notify			 = true );

		VTX::Representation::FlagDataTargeted getFlagDataTargeted() const
		{
			return VTX::Representation::getFlagDataTargeted( _linkedRepresentation->getRepresentationType() );
		};

		bool			   hasToDrawSphere() const { return _linkedRepresentation->getData().hasToDrawSphere(); };
		const SphereData & getSphereData() const { return _sphereData.getValue(); };
		void			   setSphereRadius( const float p_radius, const bool p_notify = true );

		bool				 hasToDrawCylinder() const { return _linkedRepresentation->getData().hasToDrawCylinder(); };
		const CylinderData & getCylinderData() const { return _cylinderData.getValue(); };
		void				 setCylinderRadius( const float p_radius, const bool p_notify = true );
		void				 setCylinderColorBlendingMode( const Generic::COLOR_BLENDING_MODE & p_colorBlendingMode,
														   const bool							p_notify = true );

		bool			   hasToDrawRibbon() const { return _linkedRepresentation->getData().hasToDrawRibbon(); };
		const RibbonData & getRibbonData() const { return _ribbonData.getValue(); };
		void			   setRibbonColorMode( const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode,
											   const bool									   p_notify = true );
		void			   setRibbonColorBlendingMode( const Generic::COLOR_BLENDING_MODE & p_colorBlendingMode,
													   const bool							p_notify = true );

		bool			hasToDrawSES() const { return _linkedRepresentation->getData().hasToDrawSES(); };
		const SESData & getSESData() const { return _sesData.getValue(); };
		void			setSESResolution( const float, const bool p_notify = true );

		void applyData( const InstantiatedRepresentation & p_source,
						const MEMBER_FLAG &				   p_flag,
						const bool						   p_recomputeBuffers = true,
						const bool						   p_notify			  = true );

		const Generic::REPRESENTATION & getRepresentationType() const
		{
			return _linkedRepresentation->getRepresentationType();
		}
		const Representation * const getLinkedRepresentation() const { return _linkedRepresentation; }

		void		copy( const InstantiatedRepresentation & p_source );
		MEMBER_FLAG getOverridedMembersFlag() const;
		bool		isMemberOverrided( const MEMBER_FLAG & p_member ) const;
		void		refreshSourceValues();
		void		onLinkedRepresentationChange( const Event::VTXEvent * const p_event );

	  protected:
		InstantiatedRepresentation( const Representation * const p_linkedRepresentation );
		InstantiatedRepresentation();
		~InstantiatedRepresentation();

		const Representation * _linkedRepresentation = nullptr;

		Generic::BaseRepresentable * _target = nullptr;

		Generic::OverridableParameter<Color::Rgba>		   _color;
		Generic::OverridableParameter<Generic::COLOR_MODE> _colorMode;

		Generic::OverridableParameter<SphereData>	_sphereData;
		Generic::OverridableParameter<CylinderData> _cylinderData;
		Generic::OverridableParameter<RibbonData>	_ribbonData;
		Generic::OverridableParameter<SESData>		_sesData;

		virtual void _onDataChange() {}

		void _updateTarget( const VTX::Representation::MoleculeComputationFlag & p_flag );
	};

} // namespace VTX::Model::Representation
#endif
