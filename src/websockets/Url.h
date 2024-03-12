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

#ifndef OPENOCPP_URL_H
#define OPENOCPP_URL_H

#include <string>

namespace ocpp
{
namespace websockets
{

/** @brief Helper class for URL manipulation */
class Url
{
  public:
    /** @brief Default constructor */
    Url();

    /**
     * @brief Constructor
     * @param url URL to parse
     */
    Url(const std::string& url);

    /**
     * @brief Copy constructor
     * @param copy Object to copy
     */
    Url(const Url& copy) = default;

    /** @brief Destructor */
    virtual ~Url();

    /**
     * @brief Get the URL
     * @return URL
     */
    const std::string& url() const { return m_url; }

    /**
     * @brief Indicate if the URL is valid
     * @return true if the URL is valid, false otherwise
     */
    bool isValid() const { return m_is_valid; }

    /**
     * @brief Get the protocol part of the URL
     * @return Protocol part of the URL
     */
    const std::string& protocol() const { return m_protocol; }

    /**
     * @brief Get the username part of the URL
     * @return Username part of the URL
     */
    const std::string& username() const { return m_username; }

    /**
     * @brief Get the password part of the URL
     * @return Password part of the URL
     */
    const std::string& password() const { return m_password; }

    /**
     * @brief Get the address part of the URL
     * @return Address part of the URL
     */
    const std::string& address() const { return m_address; }

    /**
     * @brief Get the port part of the URL
     * @return Port part of the URL if valid, 0 otherwise
     */
    unsigned int port() const { return m_port; }

    /**
     * @brief Get the path part of the URL
     * @return Path part of the URL
     */
    const std::string& path() const { return m_path; }

    /** @brief Encode a part of an URL using RFC3986 percent encoding */
    static std::string encode(const std::string& url);

  private:
    /** @brief Full URL */
    std::string m_url;
    /** @brief Indicate if the URL is valid */
    bool m_is_valid;
    /** @brief Protocol part of the URL */
    std::string m_protocol;
    /** @brief Username part of the URL */
    std::string m_username;
    /** @brief Password part of the URL */
    std::string m_password;
    /** @brief Address part of the URL */
    std::string m_address;
    /** @brief Port part of the URL */
    unsigned int m_port;
    /** @brief Path part of the URL */
    std::string m_path;
};

} // namespace websockets
} // namespace ocpp

#endif // OPENOCPP_URL_H
