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

#ifndef OPENOCPP_JSON_H
#define OPENOCPP_JSON_H

// Disable GCC warnings
#ifdef __GNUC__
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wexceptions"
#else
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif // __GNUC__

// Disable MSVC warnings
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4061)
#pragma warning(disable : 4127)
#pragma warning(disable : 4365)
#pragma warning(disable : 4464)
#pragma warning(disable : 4996)
#pragma warning(disable : 5027)
#pragma warning(disable : 26451)
#pragma warning(disable : 26819)
#pragma warning(disable : 33010)
#endif // _MSC_VER

// Use exception for critical parse errors
// instead of asserts
#include <stdexcept>
namespace rapidjson
{
/** @brief Specific exception class dedicated to critical parse errors */
class parse_exception : public std::logic_error
{
  public:
    /** @brief Default constructor */
    parse_exception(const char* reason) : std::logic_error(reason) { }
};
} // namespace rapidjson
#define EXCEPTION_REASON_STRINGIFY(x) #x
#define RAPIDJSON_ASSERT_THROWS
#define RAPIDJSON_ASSERT(x) \
    if (!(x))               \
    throw parse_exception(EXCEPTION_REASON_STRINGIFY(x))

// Include rapidjson's headers
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

// Restore GCC warnings
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC__

// Restore MSVC warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // OPENOCPP_JSON_H
