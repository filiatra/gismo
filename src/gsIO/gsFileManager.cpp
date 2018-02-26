/** @file gsFileManager.cpp

    @brief Utility class for finding files and handling paths

    This file is part of the G+Smo library.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

    Author(s): S. Takacs, A. Mantzaflaris, H. Weiner
*/

#include <gsIO/gsFileManager.h>
#include <iostream>
#include <fstream>
#include <gsCore/gsConfig.h>
#include <gsUtils/gsUtils.h>
#include <cstdlib>

#if defined _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <sys/stat.h>
#include <dlfcn.h>
#include <unistd.h>
#endif

namespace gismo
{

class gsFileManagerData;

// Use a singleton to store data:
class GISMO_EXPORT gsFileManagerData
{
public:

    friend gsFileManagerData& gsFileManagerDataSingleton();
    friend class gsFileManager;

    void setSearchPaths(const std::string& paths);

private:
    gsFileManagerData()
    {
#ifdef GISMO_SEARCH_PATHS
        setSearchPaths("" GISMO_SEARCH_PATHS);
#endif
    }

    gsFileManagerData(const gsFileManagerData&);
    gsFileManagerData& operator= (const gsFileManagerData&);
    std::vector<std::string> m_paths;
};

gsFileManagerData& gsFileManagerDataSingleton()
{
    static gsFileManagerData singleton;
    return singleton;
}

bool gsFileManager::fileExists(const std::string& name)
{
    return !find(name).empty();
}

bool gsFileManager::fileExistsInDataDir(const std::string& name)
{
    return !findInDataDir(name).empty();
}

bool gsFileManager::fileNotPathExists(const std::string& fn)
{
   // Note:
   //   std::ifstream s(fn.c_str()); return s.good() && ! s.eof();
   // is also possible; however that treats empty files as non-existing.
#if defined _WIN32
    DWORD dwAttrib = GetFileAttributes( fn.c_str() );
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat buf;
    return ( (0==stat(fn.c_str(), &buf)) && (0!=S_ISREG(buf.st_mode)) );
#endif
}

char gsFileManager::getNativePathSeparator()
{
#if defined _WIN32 || defined __CYGWIN__
    return '\\';
#else
    return '/';
#endif
}

bool gsFileManager::isFullyQualified(const std::string& fn)
{
#if defined _WIN32
    return util::starts_with(fn,"/")
        || util::starts_with(fn,"\\")
        || ( fn.size() > 2 && fn[1] == ':' && ( fn[2] == '/' || fn[2] == '\\' ) );
#else
    return util::starts_with(fn,"/");
#endif
}

bool gsFileManager::isRelative(const std::string& fn)
{
#if defined _WIN32
    return util::starts_with(fn,"./")
        || util::starts_with(fn,".\\")
        || util::starts_with(fn,"../")
        || util::starts_with(fn,"..\\");
#else
    return util::starts_with(fn,"./")
        || util::starts_with(fn,"../");
#endif
}

void _replace_slash_by_basckslash(std::string& str)
{
    for ( std::string::iterator it=str.begin(); it!=str.end(); it++ )
        if ( *it=='/' ) *it = '\\';
}

void gsFileManager::setSearchPaths(const std::string& paths)
{
    gsFileManagerDataSingleton().setSearchPaths(paths);
}

void gsFileManagerData::setSearchPaths(const std::string& paths)
{
    m_paths.clear();

    std::string::const_iterator a;
    std::string::const_iterator b = paths.begin();
    while (true)
    {
        a = b;
        while (b != paths.end() && (*b) != ';') { ++b; }

        std::string p(a,b);

#if defined _WIN32
        _replace_slash_by_basckslash(p);
#endif

        if (!p.empty())
        {
#if defined _WIN32
            if (*p.rbegin() != '\\')
                p.push_back('\\');
#else
            if (*p.rbegin() != '/')
                p.push_back('/');
#endif

            m_paths.push_back(p);
        }

        if ( b == paths.end() ) break;

        ++b;
    }
}

std::string gsFileManager::getSearchPaths()
{
    std::string result;
    gsFileManagerData& dat = gsFileManagerDataSingleton();
    for (std::vector<std::string>::const_iterator it = dat.m_paths.begin();
            it < dat.m_paths.end(); ++it)
    {
        result += (*it) + ";";
    }
    return result;
}

std::string gsFileManager::find(std::string fn)
{
#if defined _WIN32
    _replace_slash_by_basckslash(fn);
#endif

    if ( fileNotPathExists(fn) ) return fn;

    if ( isFullyQualified(fn) || isRelative(fn) ) return std::string();

    gsFileManagerData& dat = gsFileManagerDataSingleton();

    std::string tmp;
    for (std::vector<std::string>::const_iterator it = dat.m_paths.begin();
            it < dat.m_paths.end(); ++it)
    {
        tmp = (*it) + fn;
        if ( fileNotPathExists(tmp) )
            return tmp;
    }

    return std::string();
}

std::string gsFileManager::findInDataDir(std::string fn)
{
#if defined _WIN32
    _replace_slash_by_basckslash(fn);
#endif

    // We know that GISMO_DATA_DIR ends with a path seperator, but
    // maybe the user does not know it.
    if ( fn[0] == '/' || fn[0] == '\\' ) fn.erase(0,1);

    std::string fn_out = GISMO_DATA_DIR + fn;

    if ( fileNotPathExists(fn_out) ) return fn_out;

    return std::string();
}

bool gsFileManager::mkdir( std::string fn )
{
#if defined _WIN32
    _replace_slash_by_basckslash(fn);
    return 0!=CreateDirectory(fn.c_str(),NULL);
#else
    return ::mkdir(fn.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
}

std::string gsFileManager::getTempPath()
{
#       if   defined(_WIN32)
    TCHAR _temp[MAX_PATH];
    (void)GetTempPath(MAX_PATH, // length of the buffer
            _temp);// buffer for path
    return std::string(_temp);
#       else

    // Typically, we should consider TMPDIR
    //   http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap08.html#tag_08_03
    //   https://en.wikipedia.org/wiki/TMPDIR&oldid=728654758
    char * _temp = getenv ("TMPDIR");
    // getenv returns NULL ptr if the variable is unknown (http://en.cppreference.com/w/cpp/utility/program/getenv).
    // If it is an empty string, we should also exclude it.
    if (_temp != NULL && _temp[0] != '\0')
    {
        // note: env variable needs no free
        return std::string(_temp);
    }

    // Okey, if first choice did not work, try this:
    _temp = getenv("TEMP");
    if (_temp != NULL && _temp[0] != '\0')
    {
        // note: env variable needs no free
        return std::string(_temp);
    }
    
    // And as third choice, use just current directory
    // http://man7.org/linux/man-pages/man2/getcwd.2.html
    _temp = getcwd(NULL, 0);
    GISMO_ASSERT(NULL!=_temp, "getcwd returned NULL.");
    std::string path(_temp);
    // The string is allocated using malloc, see the reference above
    std::free(_temp);
    return path;
#       endif
}

bool gsFileManager::pathEqual( const std::string& p1, const std::string& p2 )
{
    const size_t sz = p1.size();

    if (sz != p2.size())
        return false;

    for (size_t i=0; i<sz; ++i)
    {
        if (!(
            p1[i] == p2[i]
            || ( p1[i] == '/' && p2[i] == '\\' )
            || ( p1[i] == '\\' && p2[i] == '/' )
        )) return false;

    }
    return true;

}

std::string gsFileManager::getExtension(std::string const & fn)
{
    if(fn.find_last_of(".") != std::string::npos)
    {
        std::string ext = fn.substr(fn.rfind(".")+1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        return ext;
    }
    return "";
}

std::string gsFileManager::getBasename(std::string const & fn)
{
    if(fn.find_last_of(".") != std::string::npos)
    {
        std::size_t pos1 = fn.find_last_of("/\\");
        std::size_t pos2 = fn.rfind(".");
        std::string name = fn.substr(pos1+1, pos2-pos1-1);
        return name;
    }
    return fn;
}

std::string gsFileManager::getFilename(std::string const & fn)
{
    std::size_t pos1 = fn.find_last_of("/\\");
    if(pos1 != std::string::npos)
    {
        std::string name = fn.substr(pos1+1);
        return name;
    }
    return fn;
}

// todo: return bool for success/failure
void gsFileManager::open(const std::string & fn)
{

#if defined(__APPLE__)
    const int ret = std::system( ("open " + fn + " &").c_str() );
#elif defined(__linux__)
    const int ret = std::system( ("xdg-open " + fn + " &").c_str() );
#elif defined(_WIN32)
    HINSTANCE hi = ShellExecute(GetDesktopWindow(), "open", fn.c_str(),
                                NULL, NULL, SW_SHOWNORMAL);
    const bool ret = !( (INT_PTR)hi>32);
#else
    GISMO_STATIC_ASSERT(0,"Platform not identified");
#endif
    //return ret;
    if (0!=ret)
        gsWarn<<"\nFailed to open file "<<fn<<
            " using OS preferred application.\n\n";
}

} //namespace gismo
