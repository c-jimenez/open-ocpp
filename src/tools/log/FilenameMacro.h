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

#ifndef OPENOCPP_FILENAMEMACRO_H
#define OPENOCPP_FILENAMEMACRO_H

//
// __FILENAME__ macro to retrieve the file name without its path
// all treatments to extract file name is done at compile time
// since it is using only constexpr functions
//
// Code taken from :
// https://stackoverflow.com/questions/8487986/file-macro-shows-full-path
//

#include <cstddef>

namespace ocpp
{
namespace log
{

/** @brief Compute the position of the file name in the whole file path */
template <typename T, size_t S>
inline constexpr size_t get_file_name_offset(const T (&str)[S], size_t i = S - 1)
{
    return (str[i] == '/' || str[i] == '\\') ? i + 1 : (i > 0 ? get_file_name_offset(str, i - 1) : 0);
}

/** @brief Compute the position of the file name in the whole file path for empty strings (end the recurrency) */
template <typename T>
inline constexpr size_t get_file_name_offset(T (&str)[1])
{
    (void)str;
    return 0;
}

/** @brief Helper struct to trigger constexpr filename extraction */
template <typename T, T v>
struct const_expr_value
{
    /** @brief Extracted filename */
    static constexpr const T value = v;
};

/** @brief First level macro to call the constexpr functions */
#define UTILITY_CONST_EXPR_VALUE(exp) ::ocpp::log::const_expr_value<decltype(exp), exp>::value

// Remove any previous declaration
#ifdef __FILENAME__
#undef __FILENAME__
#endif

/** @brief New filename macro */
#define __FILENAME__ (&__FILE__[UTILITY_CONST_EXPR_VALUE(::ocpp::log::get_file_name_offset(__FILE__))])

} // namespace log
} // namespace ocpp

#endif // OPENOCPP_FILENAMEMACRO_H
