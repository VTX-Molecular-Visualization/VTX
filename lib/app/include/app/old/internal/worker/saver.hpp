#ifndef __VTX_APP_OLD_INTERNAL_WORKER_SAVER__
#define __VTX_APP_OLD_INTERNAL_WORKER_SAVER__

#include "app/old/core/worker/base_thread.hpp"
#include <util/types.hpp>

namespace VTX::App::Old::Internal::Worker
{
	class Saver : public App::Old::Core::Worker::BaseThread
	{
		// Q_OBJECT

		enum class MODE : int
		{
			MOLECULE,
			VTX,
			UNKNOWN,
		};

	  public:
		explicit Saver( const FilePath & p_path ) : _path( p_path ) {}
		~Saver() {}

	  protected:
		uint _run() override;

	  private:
		const FilePath _path;

		bool _saveMolecule();
		bool _saveSession();

		MODE _getMode( const FilePath & ) const;
	};

} // namespace VTX::App::Old::Internal::Worker
#endif
