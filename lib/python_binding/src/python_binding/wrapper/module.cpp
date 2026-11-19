#include "python_binding/wrapper/module.hpp"
#include "python_binding/binding/vtx_module.hpp"
#include <pybind11/eval.h>
#include <pybind11/pybind11.h>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_set>
#include <util/logger.hpp>
#include <utility>
#include <vector>

namespace VTX::PythonBinding::Wrapper
{
	namespace
	{
		class CompletionTableBuilder
		{
		  public:
			explicit CompletionTableBuilder( const Module::CompletionVisitor & p_visitor ) : _visitor( p_visitor ) {}

			void build( const std::string & p_moduleName )
			{
				_rootName = p_moduleName;
				_visitModule( pybind11::module_::import( p_moduleName.c_str() ), p_moduleName );

				while ( not _pending.empty() )
				{
					const std::string name = std::move( _pending.back() );
					_pending.pop_back();

					const pybind11::object type = _resolve( name );
					if ( not type.is_none() )
					{
						_visitClass( type );
					}
				}
			}

		  private:
			pybind11::module_ _builtins		 = pybind11::module_::import( "builtins" );
			pybind11::object  _getattrStatic = pybind11::module_::import( "inspect" ).attr( "getattr_static" );
			pybind11::object  _property		 = _builtins.attr( "property" );
			pybind11::dict	  _modules = pybind11::module_::import( "sys" ).attr( "modules" ).cast<pybind11::dict>();

			std::string						  _rootName;
			std::unordered_set<std::string>	  _visited;
			std::vector<std::string>		  _pending;
			const Module::CompletionVisitor & _visitor;

			std::vector<std::string> _publicNames( const pybind11::handle p_object ) const
			{
				std::vector<std::string> names;
				for ( const pybind11::handle value : _builtins.attr( "dir" )( p_object ) )
				{
					std::string name = value.cast<std::string>();
					if ( not name.starts_with( '_' ) )
					{
						names.emplace_back( std::move( name ) );
					}
				}
				return names;
			}

			static std::string _typeName( const pybind11::handle p_type )
			{
				const std::string module		= p_type.attr( "__module__" ).cast<std::string>();
				const std::string qualifiedName = p_type.attr( "__qualname__" ).cast<std::string>();
				return module == "builtins" ? qualifiedName : module + "." + qualifiedName;
			}

			std::string _returnType( const pybind11::handle p_object )
			{
				static const std::regex returnPattern( R"(\)\s*->\s*([A-Za-z_][\w.]*(?:\[[\w.,\[\] ]*\])?))" );

				const pybind11::object doc = pybind11::getattr( p_object, "__doc__", pybind11::none() );
				if ( not pybind11::isinstance<pybind11::str>( doc ) )
				{
					return {};
				}

				const std::string text = doc.cast<std::string>();
				std::smatch		  match;
				if ( not std::regex_search( text, match, returnPattern ) )
				{
					return {};
				}

				std::string type = match[ 1 ].str();
				_pending.push_back( type );
				return type;
			}

			void _visitClass( const pybind11::handle p_class )
			{
				const std::string key = _typeName( p_class );
				if ( not _visited.insert( key ).second )
				{
					return;
				}

				for ( const std::string & name : _publicNames( p_class ) )
				{
					pybind11::object member;
					try
					{
						member = _getattrStatic( p_class, name );
					}
					catch ( const pybind11::error_already_set & )
					{
						continue;
					}

					const pybind11::object target = pybind11::isinstance( member, _property )
														? pybind11::object( member.attr( "fget" ) )
														: member;
					_visitor( key, name, _returnType( target ) );
				}
			}

			void _visitModule( const pybind11::handle p_module, const std::string & p_key )
			{
				if ( not _visited.insert( p_key ).second )
				{
					return;
				}

				for ( const std::string & name : _publicNames( p_module ) )
				{
					const pybind11::object member = pybind11::getattr( p_module, name.c_str(), pybind11::none() );
					if ( pybind11::isinstance<pybind11::module_>( member ) )
					{
						if ( not member.attr( "__name__" ).cast<std::string>().starts_with( _rootName ) )
						{
							continue;
						}

						const std::string subKey = p_key + "." + name;
						_visitor( p_key, name, subKey );
						_visitModule( member, subKey );
					}
					else if ( PyType_Check( member.ptr() ) )
					{
						_visitor( p_key, name, _typeName( member ) );
						_visitClass( member );
					}
					else if ( PyCallable_Check( member.ptr() ) )
					{
						_visitor( p_key, name, _returnType( member ) );
					}
					else
					{
						const pybind11::handle type = pybind11::type::handle_of( member );
						_visitor( p_key, name, _typeName( type ) );
						if ( type.attr( "__module__" ).cast<std::string>() != "builtins" )
						{
							_visitClass( type );
						}
					}
				}
			}

			pybind11::object _resolve( std::string p_name ) const
			{
				for ( const std::string_view wrapper : { "list[", "List[", "Optional[" } )
				{
					if ( p_name.starts_with( wrapper ) && p_name.ends_with( ']' ) )
					{
						p_name = p_name.substr( wrapper.size(), p_name.size() - wrapper.size() - 1 );
					}
				}

				const size_t separator = p_name.rfind( '.' );
				if ( separator == std::string::npos )
				{
					return pybind11::none();
				}

				const std::string modulePath = p_name.substr( 0, separator );
				if ( not _modules.contains( modulePath ) )
				{
					return pybind11::none();
				}

				const pybind11::object member = pybind11::getattr(
					_modules[ modulePath.c_str() ], p_name.substr( separator + 1 ).c_str(), pybind11::none()
				);
				return PyType_Check( member.ptr() ) ? member : pybind11::none();
			}
		};
	} // namespace

	Module Module::defSubmodule( const std::string & p_submoduleName, const std::string & p_submoduleDesc )
	{
		pybind11::module_ module	 = _pyModule.def_submodule( p_submoduleName.c_str(), p_submoduleDesc.c_str() );
		const std::string modulePath = _modulePath + "." + p_submoduleName;

		return Module( module, modulePath );
	}

	Module Module::getSubmodule( const std::string & p_submoduleName )
	{
		const std::string submodulePath = _modulePath + "." + p_submoduleName;
		pybind11::module_ module		= _pyModule.import( submodulePath.c_str() );

		return Module( module, submodulePath );
	}

	void Module::displayInfo() const
	{
		try
		{
			pybind11::exec( "print( dir() )", _pyModule.attr( "__dict__" ) );
		}
		catch ( const pybind11::error_already_set & e )
		{
			VTX_ERROR( "Display module info fail : {}", e.what() );
		}
	}

	void Module::visitCompletionTable( const CompletionVisitor & p_visitor )
	{
		try
		{
			CompletionTableBuilder builder( p_visitor );
			builder.build( vtx_module_name() );
		}
		catch ( const pybind11::error_already_set & e )
		{
			VTX_ERROR( "visitCompletionTable fail : {}", e.what() );
		}
		catch ( const pybind11::cast_error & e )
		{
			VTX_ERROR( "visitCompletionTable fail : {}", e.what() );
		}
	}

	Module::~Module() = default;
} // namespace VTX::PythonBinding::Wrapper
