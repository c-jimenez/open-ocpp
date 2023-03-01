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

#ifndef OPENOCPP_CPCERTIFICATESDATABASE_H
#define OPENOCPP_CPCERTIFICATESDATABASE_H

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

/** @brief Handle persistency of Charge Point certificates */
class CpCertificatesDatabase
{
  public:
    /** @brief Constructor */
    CpCertificatesDatabase(const ocpp::config::IChargePointConfig& stack_config, ocpp::database::Database& database);

    /** @brief Destructor */
    virtual ~CpCertificatesDatabase();

    // CpCertificatesDatabase interface

    /** @brief Initialize the database table */
    void initDatabaseTable();

    /**
     * @brief Save a certificate request and its private key
     * @param certificate_request Certificate request
     * @param private_key Private key
     */
    void saveCertificateRequest(const ocpp::x509::CertificateRequest& certificate_request, const ocpp::x509::PrivateKey& private_key);

    /**
     * @brief Get the saved certificate request
     * @param id Id of the request
     * @return Save certificate request in PEM encoded data
     */
    std::string getCertificateRequest(unsigned int& id);

    /**
     * @brief Install a certificate
     * @param id Id of the corresponding request
     * @param certificate Certificate to install
     * @return true if the certificate has been installed, false otherwise
     */
    bool installCertificate(unsigned int request_id, const ocpp::x509::Certificate& certificate);

    /**
     * @brief Check if a valid certificate has been installed
     * @return true if there is at leat 1 valid certificate, false otherise
     */
    bool isValidCertificateInstalled();

    /**
     * @brief Get the installed Charge Point certificate as PEM encoded data
     * @param private_key Corresponding private key as PEM encoded data
     * @return Installed Charge Point certificate as PEM encoded data
     */
    std::string getChargePointCertificate(std::string& private_key);

  private:
    /** @brief Stack configuration */
    const ocpp::config::IChargePointConfig& m_stack_config;
    /** @brief Charge point's database */
    ocpp::database::Database& m_database;

    /** @brief Query to list the valid certificates */
    std::unique_ptr<ocpp::database::Database::Query> m_list_query;
    /** @brief Query to look for a certificate request */
    std::unique_ptr<ocpp::database::Database::Query> m_find_csr_query;
    /** @brief Query to delete a certificate request */
    std::unique_ptr<ocpp::database::Database::Query> m_delete_csr_query;
    /** @brief Query to insert a certificate in place of its corresponding request */
    std::unique_ptr<ocpp::database::Database::Query> m_update_csr_to_cert_query;
    /** @brief Query to insert a certificate request */
    std::unique_ptr<ocpp::database::Database::Query> m_insert_csr_query;
};

} // namespace chargepoint
} // namespace ocpp

#endif // OPENOCPP_CPCERTIFICATESDATABASE_H
