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

#ifndef OPENOCPP_OPENSSL_H
#define OPENOCPP_OPENSSL_H

// Disable MSVC warnings
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4191)
#endif // _MSC_VER

// Include OpenSSL's headers
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#if (OPENSSL_VERSION_NUMBER >= 0x30000000L)
// OpenSSL 3.x
#include <openssl/core_names.h>
#endif // OPENSSL_VERSION_NUMBER

// Restore MSVC warnings
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

#endif // OPENOCPP_OPENSSL_H
