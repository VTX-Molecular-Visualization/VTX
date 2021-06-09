#ifndef __VTX_IO_SCENE_PATH_DATA__
#define __VTX_IO_SCENE_PATH_DATA__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "event/base_event_receiver_vtx.hpp"
#include "event/event.hpp"
#include "io/writer/writer_chemfiles.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}

	namespace IO
	{
		class ScenePathData : Event::BaseEventReceiverVTX
		{
		  public:
			struct Data
			{
			  public:
				Data();
				Data( const FilePath & p_path );

				void			 registerPath( const FilePath & p_filepath );
				bool			 hasFilepath() const;
				const FilePath & getFilepath() const;

				bool hasChanged() const;
				void setHasChanged( const bool p_hasChanged );

				void							registerWriter( Writer::ChemfilesWriter * const p_writer );
				Writer::ChemfilesWriter * const getWriter();
				void							deleteWriter();

				bool needToSaveMolecule() const;

			  private:
				FilePath				  _path		  = "";
				bool					  _hasChanged = false;
				Writer::ChemfilesWriter * _writer	  = nullptr;
			};

			ScenePathData();
			virtual void receiveEvent( const Event::VTXEvent & p_event ) override;

			void registerLoading( const Model::Molecule * const p_molecule, const FilePath & p_filepath );

			Data & getData( const Model::Molecule * const p_molecule );

		  private:
			std::map<const Model::Molecule *, Data> _mapMoleculePath;
		};
	} // namespace IO
} // namespace VTX
#endif
