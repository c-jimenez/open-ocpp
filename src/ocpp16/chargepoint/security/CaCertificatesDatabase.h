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

#ifndef OPENOCPP_CACERTIFICATESDATABASE_H
#define OPENOCPP_CACERTIFICATESDATABASE_H

#include "CertificateHashDataType.h"
#include "Database.h"

#include <memory>

namespace ocpp
{
// Forward declarations
namespace config
{
class IChargePointConfig;
} // namespace config
namespace x509
{
class PrivateKey;
class Certificate;
class CertificateRequest;
} // namespace x509

// Main namespace
namespace chargepoint
{

/** @brief Handle persistency of CA certificates */
class CaCertificatesDatabase
{
  public:
    /** @brief Constructor */
    CaCertificatesDatabase(const ocpp::config::IChargePointConfig& stack_config, ocpp::database::Database& database);

    /** @brief Destructor */
    virtual ~CaCertificatesDatabase();

    // CaCertificatesDatabase interface

    /** @brief Initialize the database table */
    void initDatabaseTable();

    /**
     * @brief Delete an installed CA certificate
     * @param certificate Certificate information
     * @return Operation status (see DeleteCertificateStatusEnumType documentation)
     */
    ocpp::types::ocpp16::DeleteCertificateStatusEnumType deleteCertificate(const ocpp::types::ocpp16::CertificateHashDataType& certificate);

    /**
     * @brief Get the list of certificates
     * @param type Type of certificates
     * @param certificates List of certificates to fill
     */
    void getCertificateList(ocpp::types::ocpp16::CertificateUseEnumType                type,
                            std::vector<ocpp::types::ocpp16::CertificateHashDataType>& certificates);

    /**
     * @brief Get the list of valid certificates in PEM encoded data format
     * @param type Type of certificates
     * @return List of valid certificates in PEM encoded data format
     */
    std::string getCertificateListPem(ocpp::types::ocpp16::CertificateUseEnumType type);

    /**
     * @brief Get the number of installed certificates
     * @param type Type of certificates
     * @param check_validity Check the validity of the certificates
     * @return Number of installed certificates
     */
    unsigned int getCertificateCount(ocpp::types::ocpp16::CertificateUseEnumType type, bool check_validity);

    /**
     * @brief Add a new certificate
     * @param type Type of certificate
     * @param certificate Certificate to add
     * @param hash_data Certificate information
     * @return true if the certificate has been added, false otherwise
     */
    bool addCertificate(ocpp::types::ocpp16::CertificateUseEnumType         type,
                        const ocpp::x509::Certificate&                      certificate,
                        const ocpp::types::ocpp16::CertificateHashDataType& hash_data);

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;

    /** @brief Query to list certificates */
    std::unique_ptr<ocpp::database::Database::Query> m_list_query;
    /** @brief Query to count certificates */
    std::unique_ptr<ocpp::database::Database::Query> m_count_query;
    /** @brief Query to look for a certificate */
    std::unique_ptr<ocpp::database::Database::Query> m_find_query;
    /** @brief Query to delete a certificate */
    std::unique_ptr<ocpp::database::Database::Query> m_delete_query;
    /** @brief Query to insert a certificate */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_query;

    /** @brief Look for a certificate */
    bool findCertificate(const ocpp::types::ocpp16::CertificateHashDataType& certificate, unsigned int& id, bool& in_use);
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_CACERTIFICATESDATABASE_H
