/*
Copyright (c) 2020 Cedric Jimenez
This file is part of OpenOCPP.

OpenOCPP is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 of the License, or
(at your option) any later version.

OpenOCPP is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OpenOCPP. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef OPENOCPP_VERSION_H
#define OPENOCPP_VERSION_H

/** @brief Stack version in the form XX.YY.ZZ where
 *          XX = Major version number (increase = API compatibility broken and/or major feature addition)
 *          YY = Minor version number (increase = Minor feature addition)
 *          ZZ = Fix version number (increase = Bug fix(es))
 *
 *          Stack version number is defined in the main CMakeLists.txt file of
 *          the build tree
 */
#define OPEN_OCPP_VERSION LIBOPENOCPP_MAJOR "." LIBOPENOCPP_MINOR "." LIBOPENOCPP_FIX

/** @brief Return the version string of the library */
extern "C" const char* OPEN_OCPP_Version();

#endif // OPENOCPP_VERSION_H
