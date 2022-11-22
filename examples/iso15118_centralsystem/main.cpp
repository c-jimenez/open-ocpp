/*
MIT License

Copyright (c) 2020 Cedric Jimenez

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sel
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "CentralSystemDemoConfig.h"
#include "Certificate.h"
#include "CertificateRequest.h"
#include "DefaultCentralSystemEventsHandler.h"
#include "ICentralSystem.h"
#include "PrivateKey.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <thread>

using namespace ocpp::centralsystem;
using namespace ocpp::types;
using namespace ocpp::x509;

/** @brief Create the certificates for the ISO15118 communications */
static void createIso15118Certificates(std::filesystem::path iso_v2g_root_ca,
                                       std::filesystem::path iso_v2g_root_ca_key,
                                       std::filesystem::path iso_mo_root_ca,
                                       std::filesystem::path iso_mo_root_ca_key);

/** @brief Entry point */
int main(int argc, char* argv[])
{
    // Default parameters
    std::string working_dir = ".";
    bool        reset_all   = false;

    // Check parameters
    if (argc > 1)
    {
        const char* param     = nullptr;
        bool        bad_param = false;
        argv++;
        while ((argc != 1) && !bad_param)
        {
            if (strcmp(*argv, "-h") == 0)
            {
                bad_param = true;
            }
            else if ((strcmp(*argv, "-w") == 0) && (argc > 1))
            {
                argv++;
                argc--;
                working_dir = *argv;
            }
            else if (strcmp(*argv, "-r") == 0)
            {
                reset_all = true;
            }
            else
            {
                param     = *argv;
                bad_param = true;
            }

            // Next param
            argc--;
            argv++;
        }
        if (bad_param)
        {
            if (param)
            {
                std::cout << "Invalid parameter : " << param << std::endl;
            }
            std::cout << "Usage : iso15118_centralsystem [-w working_dir] [-r]" << std::endl;
            std::cout
                << "    -w : Working directory where to store the configuration file and the local database (Default = current directory)"
                << std::endl;
            std::cout << "    -r : Reset all the OCPP persistent data" << std::endl;
            return 1;
        }
    }

    std::cout << "Starting central system with :" << std::endl;
    std::cout << "  - working_dir = " << working_dir << std::endl;

    // Generate certificates for ISO15118
    std::filesystem::path iso_v2g_root_ca(working_dir);
    iso_v2g_root_ca /= "cs_iso_v2g_root_ca.pem";
    std::filesystem::path iso_v2g_root_ca_key(working_dir);
    iso_v2g_root_ca_key /= "cs_iso_v2g_root_ca.key";
    std::filesystem::path iso_mo_root_ca(working_dir);
    iso_mo_root_ca /= "cs_iso_mo_root_ca.pem";
    std::filesystem::path iso_mo_root_ca_key(working_dir);
    iso_mo_root_ca_key /= "cs_iso_mo_root_ca.key";
    if (reset_all)
    {
        std::filesystem::remove(iso_v2g_root_ca);
        std::filesystem::remove(iso_v2g_root_ca_key);
        std::filesystem::remove(iso_mo_root_ca);
        std::filesystem::remove(iso_mo_root_ca_key);
    }
    if (!std::filesystem::exists(iso_v2g_root_ca))
    {
        createIso15118Certificates(iso_v2g_root_ca, iso_v2g_root_ca_key, iso_mo_root_ca, iso_mo_root_ca_key);
    }

    // Instanciate certificates
    Certificate v2g_root_cert(iso_v2g_root_ca);
    Certificate mo_root_cert(iso_mo_root_ca);

    // Configuration
    std::filesystem::path path(working_dir);
    path /= "iso15118_centralsystem.ini";
    CentralSystemDemoConfig config(path);

    // Event handler
    DefaultCentralSystemEventsHandler event_handler(iso_v2g_root_ca, iso_mo_root_ca, true);

    // Instanciate central system
    std::unique_ptr<ICentralSystem> central_system = ICentralSystem::create(config.stackConfig(), event_handler);
    if (reset_all)
    {
        central_system->resetData();
    }
    central_system->start();

    // From now on the stack is alive :)

    // App loop
    while (true)
    {
        // For each pending charge point
        for (auto& iter_chargepoint : event_handler.pendingChargePoints())
        {
            auto chargepoint   = iter_chargepoint.second;
            auto iter_accepted = event_handler.acceptedChargePoints().find(chargepoint->identifier());
            if (iter_accepted == event_handler.acceptedChargePoints().end())
            {
                std::cout << "---------------------------------------------" << std::endl;
                std::cout << "Pending Charge point : " << chargepoint->identifier() << std::endl;
                std::cout << "---------------------------------------------" << std::endl;

                // Check if the chargepoint supports ISO15118
                std::vector<KeyValue>    config_keys;
                std::vector<std::string> unknown_keys;
                if (!chargepoint->getConfiguration(std::vector<std::string>{"Iso15118PnCEnabled"}, config_keys, unknown_keys))
                {
                    std::cout << "Unable to get the status of ISO15118 implementation on charge point side" << std::endl;
                }
                if (!config_keys.empty() && (config_keys[0].value.value() == "true"))
                {
                    std::cout << "Charge point supports ISO15118 PnC extensions" << std::endl;

                    // List installed certificates
                    std::vector<ocpp::types::CertificateHashDataChainType> certificates;
                    if (chargepoint->iso15118GetInstalledCertificateIds(
                            std::vector<GetCertificateIdUseEnumType>{GetCertificateIdUseEnumType::V2GRootCertificate,
                                                                     GetCertificateIdUseEnumType::MORootCertificate},
                            certificates))
                    {
                        // Delete installed certificates
                        for (const auto& certificate : certificates)
                        {
                            DeleteCertificateStatusEnumType ret = chargepoint->iso15118DeleteCertificate(certificate.certificateHashData);
                            std::cout << "Delete certificate [" << GetCertificateIdUseEnumTypeHelper.toString(certificate.certificateType)
                                      << "] => " << DeleteCertificateStatusEnumTypeHelper.toString(ret) << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Unable to list installed certificates" << std::endl;
                    }

                    // Install V2G root certificate
                    InstallCertificateStatusEnumType install_ret =
                        chargepoint->iso15118InstallCertificate(InstallCertificateUseEnumType::V2GRootCertificate, v2g_root_cert);
                    std::cout << "Install V2G root CA => " << InstallCertificateStatusEnumTypeHelper.toString(install_ret) << std::endl;

                    // Install MO root certificate
                    install_ret = chargepoint->iso15118InstallCertificate(InstallCertificateUseEnumType::MORootCertificate, mo_root_cert);
                    std::cout << "Install MO root CA => " << InstallCertificateStatusEnumTypeHelper.toString(install_ret) << std::endl;

                    // Trigger a request to sign a new V2G certificate
                    if (chargepoint->iso15118TriggerSignCertificate())
                    {
                        // Waits for the request from the charge point
                        unsigned int retries        = 0;
                        auto         my_chargepoint = event_handler.chargePoints().find(chargepoint->identifier())->second;
                        while (retries < 10u)
                        {
                            if (my_chargepoint->generatedCertificate().empty())
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                                retries++;
                            }
                            else
                            {
                                // Send generated certificate
                                Certificate certificate_chain(std::filesystem::path(my_chargepoint->generatedCertificate()));
                                if (!chargepoint->iso15118CertificateSigned(certificate_chain))
                                {
                                    std::cout << "Unable to send generated certificate : " << my_chargepoint->generatedCertificate()
                                              << std::endl;
                                }
                                break;
                            }
                        }
                        if (my_chargepoint->generatedCertificate().empty())
                        {
                            std::cout << "Sign certificate request not received" << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "Unable to trigger a sign certificate request" << std::endl;
                    }
                }
                else
                {
                    std::cout << "Charge point doesn't support ISO15118 PnC extensions" << std::endl;
                }

                // Accept charge point
                event_handler.acceptedChargePoints()[chargepoint->identifier()] = chargepoint;

                // Trigger a boot notification to force it to update its registration status
                chargepoint->triggerMessage(MessageTrigger::BootNotification, Optional<unsigned int>());
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

/** @brief Create the certificates for the ISO15118 communications */
static void createIso15118Certificates(std::filesystem::path iso_v2g_root_ca,
                                       std::filesystem::path iso_v2g_root_ca_key,
                                       std::filesystem::path iso_mo_root_ca,
                                       std::filesystem::path iso_mo_root_ca_key)
{
    // V2G root CA
    PrivateKey v2g_key(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "");
    v2g_key.privateToFile(iso_v2g_root_ca_key);

    CertificateRequest::Subject v2g_subject;
    v2g_subject.country           = "FR";
    v2g_subject.state             = "Savoie";
    v2g_subject.location          = "Chambery";
    v2g_subject.organization      = "Open OCPP";
    v2g_subject.organization_unit = "Examples";
    v2g_subject.common_name       = "V2G root CA";
    v2g_subject.email_address     = "ca.examples@open-ocpp.org";

    CertificateRequest::Extensions ca_extensions;
    ca_extensions.basic_constraints.present     = true;
    ca_extensions.basic_constraints.is_ca       = true;
    ca_extensions.basic_constraints.path_length = 1u;
    ca_extensions.subject_alternate_names.push_back("localhost");
    ca_extensions.subject_alternate_names.push_back("127.0.0.1");

    CertificateRequest v2g_req(v2g_subject, ca_extensions, v2g_key);

    Certificate v2g_cert(v2g_req, v2g_key, Sha2::Type::SHA256, 7300u);
    v2g_cert.toFile(iso_v2g_root_ca);

    // MO root CA
    PrivateKey mo_key(PrivateKey::Type::EC, PrivateKey::Curve::PRIME256_V1, "");
    mo_key.privateToFile(iso_mo_root_ca_key);

    CertificateRequest::Subject mo_subject;
    mo_subject.country           = "FR";
    mo_subject.state             = "Savoie";
    mo_subject.location          = "Chambery";
    mo_subject.organization      = "Open OCPP";
    mo_subject.organization_unit = "Examples";
    mo_subject.common_name       = "MO root CA";
    mo_subject.email_address     = "ca.examples@open-ocpp.org";

    CertificateRequest mo_req(mo_subject, ca_extensions, mo_key);

    Certificate mo_cert(mo_req, mo_key, Sha2::Type::SHA256, 7300u);
    mo_cert.toFile(iso_mo_root_ca);
}
