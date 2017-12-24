/** @file gsFileManager.h

    @brief Utility class for finding files and handling paths

    This file is part of the G+Smo library.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

    Author(s): S. Takacs
*/

#pragma once

#include <gsCore/gsForwardDeclarations.h>

namespace gismo 
{

/// @brief This class checks if the given filename can be found
///  in one of the pre-defined search paths. It is possible to
///  register additional search paths.
///
/// @ingroup IO
class GISMO_EXPORT gsFileManager
{
public:

    /// Checks if the file exists (also in the search paths)
    static bool fileExists(const std::string& name);

    /// Get local path seperator
    static char getLocalPathSeperator();

    /// Checks if the path is fully qualified
    /// If a name starts with "/", it is considered fully qualified
    static bool isFullyQualified(const std::string& fn);

    /// Checks if the path is a relative path
    /// If a name starts with "./" or "../", it is considered fully qualified
    static bool isRelative(const std::string& fn);

    /// Set the search paths
    static void setSearchPaths(const std::string& paths);

    /// Get the defined search path
    static std::string getSearchPaths();

    /// \brief Find a file.
    ///
    /// \param[in] fn The filename
    /// \param[out] the full path or empty string
    ///
    /// If the file can be found, returns the full path.
    /// Otherwiese, returns empty string.
    ///
    /// If \a fn satisfied \a isFullyQualified or \a isRelative, it is kept unchanged
    static std::string find(const std::string fn );

    /// Make directory
    static bool mkdir( std::string fn );

    /// Checks paths for equality, ignoring slash vs. backslash
    static bool pathEqual( const std::string& p1, const std::string& p2 );
};


} // namespace gismo
