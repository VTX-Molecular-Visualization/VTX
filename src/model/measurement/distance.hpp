#ifndef __VTX_MODEL_MEASUREMENT_DISTANCE__
#define __VTX_MODEL_MEASUREMENT_DISTANCE__

#include "event/base_event_receiver_vtx.hpp"
#include "id.hpp"
#include "model/label.hpp"
#include <string>
#include <utility>

namespace VTX::Model
{
	class Atom;
} // namespace VTX::Model

namespace VTX::Model::Measurement
{
	class Distance : public Model::Label, Event::BaseEventReceiverVTX
	{
		VTX_MODEL

	  public:
		using AtomPair = std::pair<const Model::Atom &, const Model::Atom &>;

	  public:
		void setAtoms( const Model::Atom & p_firstAtom, const Model::Atom & p_secondAtom, const bool p_notify = true );
		void receiveEvent( const Event::VTXEvent & p_event ) override;

		const Model::Atom & getFirstAtom() const { return *_firstAtom; }
		const Model::Atom & getSecondAtom() const { return *_secondAtom; }

		float getDistance() const { return _distance; }
		void  displayInLog() const;

	  protected:
		Distance();
		Distance( const AtomPair & p_pair );

	  private:
		const Model::Atom * _firstAtom	= nullptr;
		const Model::Atom * _secondAtom = nullptr;

		float _distance = 0.f;

		void _computeDistance( const bool p_notify = true );
	};

} // namespace VTX::Model::Measurement
#endif
