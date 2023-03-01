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

#ifndef OPENOCPP_SHA2_H
#define OPENOCPP_SHA2_H

#include <cstdint>
#include <string>
#include <vector>

namespace ocpp
{
namespace x509
{

/** @brief Compute SHA-2 secure hashes */
class Sha2
{
  public:
    /** @brief SHA-2 types */
    enum Type
    {
        /** @brief SHA-256 */
        SHA256,
        /** @brief SHA-384 */
        SHA384,
        /** @brief SHA-512 */
        SHA512
    };

    /**
     * @brief Constructor
     * @param type Type of SHA to compute
     */
    Sha2(Type type = Type::SHA256);

    /** @brief Destructor */
    virtual ~Sha2();

    /**
     * @brief Compute the SHA of a data buffer
     *        => Does init + update + finalize on the buffer
     * @param data Data buffer
     * @param size Size of the data buffer in bytes
     */
    std::vector<uint8_t> compute(const void* data, size_t size);

    /** @brief Initialize a new computation */
    void init();

    /**
     * @brief Add data to the current computation
     * @param data Data to add
     * @param size Size of the data to add in bytes
     */
    void update(const void* data, size_t size);

    /**
     * @brief Finalize the computation
     * @return SHA computed
     */
    std::vector<uint8_t> finalize();

    /**
     * @brief Get the last computation result
     * @return Last computation result
     */
    const std::vector<uint8_t>& result() const { return m_last_result; }

    /**
     * @brief Get the last computation result as an hexadecimal string
     * @return Last computation result as an hexadecimal string
     */
    std::string resultString() const;

  private:
    /** @brief Computation context */
    void* m_context;
    /** @brief Hash algorithm */
    const void* m_md;
    /** @brief Last computation result */
    std::vector<uint8_t> m_last_result;
};

} // namespace x509
} // namespace ocpp

#endif // OPENOCPP_SHA2_H
