#include "filesystem.hpp"
#include <gvl/support/platform.hpp>
#include <stdexcept>
#include <cassert>
#include <cctype>

std::string changeLeaf(std::string const& path, std::string const& newLeaf)
{
	std::size_t lastSep = path.find_last_of("\\/");
	
	if(lastSep == std::string::npos)
		return newLeaf; // We assume there's only a leaf in the path
	return path.substr(0, lastSep + 1) + newLeaf;
}

std::string getRoot(std::string const& path)
{
	std::size_t lastSep = path.find_last_of("\\/");
	
	if(lastSep == std::string::npos)
		return "";
	return path.substr(0, lastSep);
}

std::string getLeaf(std::string const& path)
{
	std::size_t lastSep = path.find_last_of("\\/");
	
	if(lastSep == std::string::npos)
		return path;
	return path.substr(lastSep + 1);
}

std::string getBasename(std::string const& path)
{
	std::size_t lastSep = path.find_last_of(".");
	
	if(lastSep == std::string::npos)
		return path;
	return path.substr(0, lastSep);
}

std::string getExtension(std::string const& path)
{
	std::size_t lastSep = path.find_last_of(".");
	
	if(lastSep == std::string::npos)
		return "";
	return path.substr(lastSep + 1);
}

void toUpperCase(std::string& str)
{
	for(std::size_t i = 0; i < str.size(); ++i)
	{
		str[i] = std::toupper(static_cast<unsigned char>(str[i])); // TODO: Uppercase conversion that works for the DOS charset
	}
}

void toLowerCase(std::string& str)
{
	for(std::size_t i = 0; i < str.size(); ++i)
	{
		str[i] = std::tolower(static_cast<unsigned char>(str[i])); // TODO: Lowercase conversion that works for the DOS charset
	}
}

FILE* tolerantFOpen(std::string const& name, char const* mode)
{
	FILE* f = std::fopen(name.c_str(), mode);
	if(f)
		return f;
		
	std::string ch(name);
	toUpperCase(ch);
	f = std::fopen(ch.c_str(), mode);
	if(f)
		return f;
		
	toLowerCase(ch);
	f = std::fopen(ch.c_str(), mode);
	if(f)
		return f;
		
	// Try with first letter capital
	ch[0] = std::toupper(static_cast<unsigned char>(ch[0]));
	f = std::fopen(ch.c_str(), mode);
	if(f)
		return f;
		
	return 0;
}

std::string joinPath(std::string const& root, std::string const& leaf)
{
	if(!root.empty()
	&& root[root.size() - 1] != '\\'
	&& root[root.size() - 1] != '/')
	{
		return root + '/' + leaf;
	}
	else
	{
		return root + leaf;
	}
}

bool fileExists(std::string const& path)
{
	FILE* f = fopen(path.c_str(), "rb");
	bool state = (f != 0);
	if(f) fclose(f);
	return state;
}

std::size_t fileLength(FILE* f)
{
	long old = ftell(f);
	fseek(f, 0, SEEK_END);
	long len = ftell(f);
	fseek(f, old, SEEK_SET);
	return len;
}

#if GVL_WINDOWS
#  include "windows.h"

#  if defined(__BORLANDC__) || defined(__MWERKS__)
#     if defined(__BORLANDC__)
        using std::time_t;
#     endif
#     include "utime.h"
#   else
#     include "sys/utime.h"
#   endif
# else
#   include "dirent.h"
#   include "unistd.h"
#   include "fcntl.h"
#   include "utime.h"
#   include <errno.h>
# endif

namespace
{

struct filename_result
{
	filename_result()
	: name(0)
	{
	}
	
	filename_result(char const* name)
	: name(name)
	{
	
	}
	
	operator void const*()
	{
		return name;
	}
	
	char const* name;
};

//#if GVL_LINUX || __APPLE__ 

# define BOOST_HANDLE DIR *
# define BOOST_INVALID_HANDLE_VALUE 0
# define BOOST_SYSTEM_DIRECTORY_TYPE struct dirent *

inline filename_result find_first_file( const char * dir,
BOOST_HANDLE & handle, BOOST_SYSTEM_DIRECTORY_TYPE & )
// Returns: 0 if error, otherwise name
{
	const char * dummy_first_name = ".";
	return ( (handle = ::opendir( dir ))
		== BOOST_INVALID_HANDLE_VALUE ) ? filename_result() : filename_result(dummy_first_name);
}  

inline void find_close( BOOST_HANDLE handle )
{
	assert( handle != BOOST_INVALID_HANDLE_VALUE );
	::closedir( handle );
}

inline filename_result find_next_file(
BOOST_HANDLE handle, BOOST_SYSTEM_DIRECTORY_TYPE & )
// Returns: if EOF 0, otherwise name
// Throws: if system reports error
{

	//  TODO: use readdir_r() if available, so code is multi-thread safe.
	//  Fly-in-ointment: not all platforms support readdir_r().

	struct dirent * dp;
	errno = 0;
	if ( (dp = ::readdir( handle )) == 0 )
	{
		if ( errno != 0 )
		{
			throw std::runtime_error("Error iterating directory");
		}
		else { return filename_result(); } // end reached
	}
	return filename_result(dp->d_name);
}
/*
#elif GVL_WINDOWS

# define BOOST_HANDLE HANDLE
# define BOOST_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
# define BOOST_SYSTEM_DIRECTORY_TYPE WIN32_FIND_DATAA


inline filename_result find_first_file( const char * dir,
BOOST_HANDLE & handle, BOOST_SYSTEM_DIRECTORY_TYPE & data )
// Returns: 0 if error, otherwise name
{
	//    std::cout << "find_first_file " << dir << std::endl;
	std::string dirpath( std::string(dir) + "/*" );
	bool fail = ( (handle = ::FindFirstFileA( dirpath.c_str(), &data )) == BOOST_INVALID_HANDLE_VALUE );
	
	if(fail)
		return filename_result();
	
	return filename_result(data.cFileName);
}  

inline void find_close( BOOST_HANDLE handle )
{
	//    std::cout << "find_close" << std::endl;
	assert( handle != BOOST_INVALID_HANDLE_VALUE );
	::FindClose( handle );
}

inline filename_result find_next_file(
BOOST_HANDLE handle, BOOST_SYSTEM_DIRECTORY_TYPE & data )
// Returns: 0 if EOF, otherwise name
// Throws: if system reports error
{
	if ( ::FindNextFileA( handle, &data ) == 0 )
	{
		if ( ::GetLastError() != ERROR_NO_MORE_FILES )
		{
			throw std::runtime_error("Error iterating directory");
		}
		else { return filename_result(); } // end reached
	}
	
	return filename_result(data.cFileName);
}

#else

#error "Not supported"
#endif
*/
}

#if GVL_WINDOWS

bool create_directories(std::string const& dir)
{
	for (std::size_t i = 0; i < dir.size(); ++i)
	{
		if (dir[i] == '\\' || dir[i] == '/')
		{
			std::string path = dir.substr(0, i);
			DWORD attr = GetFileAttributesA(path.c_str());

			if (attr == INVALID_FILE_ATTRIBUTES)
			{
				CreateDirectoryA(path.c_str(), NULL);
			}
			else if (!(attr & FILE_ATTRIBUTE_DIRECTORY))
			{
				return false;
			}
		}
	}

	return true;
}

#else

#include <sys/types.h>
#include <sys/stat.h>

bool create_directories(std::string const& dir)
{
	for (std::size_t i = 1; i < dir.size(); ++i)
	{
		if (dir[i] == '/')
		{
			std::string path = dir.substr(0, i);
            struct stat s;
            if (stat(path.c_str(), &s) < 0)
            {
                if (mkdir(path.c_str(), 0777) < 0)
                {
                    return false;
                }
            }
		}
	}
    
	return true;
}

#endif

struct dir_itr_imp
{
public:
	dir_itr_imp()
	{
		
	}
	
	std::string       entry_path;
	BOOST_HANDLE      handle;

	~dir_itr_imp()
	{
		if ( handle != BOOST_INVALID_HANDLE_VALUE )
			find_close( handle );
	}
};

DirectoryIterator::DirectoryIterator(std::string const& dir)
{
	dir_itr_init( m_imp, dir.empty() ? "." : dir.c_str() );
}

DirectoryIterator::~DirectoryIterator()
{
}


inline bool dot_or_dot_dot( char const * name )
{
	return name[0]=='.'
		&& (name[1]=='\0' || (name[1]=='.' && name[2]=='\0'));
}

//  directory_iterator implementation  ---------------------------------------//

void dir_itr_init( dir_itr_imp_ptr & m_imp,
	char const* dir_path )
{
	m_imp.reset( new dir_itr_imp() );
	BOOST_SYSTEM_DIRECTORY_TYPE scratch;
	filename_result name;  // initialization quiets compiler warnings

	if ( !dir_path[0] )
		m_imp->handle = BOOST_INVALID_HANDLE_VALUE;
	else
		name = find_first_file( dir_path, m_imp->handle, scratch );  // sets handle

	if ( m_imp->handle != BOOST_INVALID_HANDLE_VALUE )
	{
		if ( !dot_or_dot_dot( name.name ) )
		{ 
			m_imp->entry_path = name.name;
		}
		else
		{
			//m_imp->entry_path.m_path_append( "dummy", no_check );
			dir_itr_increment( m_imp );
		}
	}
	else
	{
		m_imp.reset();
	}  
}

std::string & dir_itr_dereference(
	const dir_itr_imp_ptr & m_imp )
{
	assert( m_imp.get() ); // fails if dereference end iterator
	return m_imp->entry_path;
}

void dir_itr_increment( dir_itr_imp_ptr & m_imp )
{
	assert( m_imp.get() ); // fails on increment end iterator
	assert( m_imp->handle != BOOST_INVALID_HANDLE_VALUE ); // reality check

	BOOST_SYSTEM_DIRECTORY_TYPE scratch;
	filename_result name;

	while ( (name = find_next_file( m_imp->handle, scratch )) )
	{
		// append name, except ignore "." or ".."
		if ( !dot_or_dot_dot( name.name ) )
		{
			m_imp->entry_path = name.name;
			return;
		}
	}
	
	m_imp.reset(); // make base() the end iterator
}
