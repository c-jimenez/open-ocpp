#!/bin/bash

echo "Generating private key for CA..."
openssl ecparam -name prime256v1 -out open-ocpp_ca.param
openssl ecparam -in open-ocpp_ca.param -genkey -noout -out open-ocpp_ca.key
echo "Generating certificate for CA..."
openssl req -new -sha256 -key open-ocpp_ca.key -extensions v3_ca -config open-ocpp_ca.cnf -out open-ocpp_ca.csr
echo "Self-signing CA certificate..."
openssl x509 -req -sha256 -days 3650 -in open-ocpp_ca.csr -extensions v3_ca -extfile open-ocpp_ca.cnf -signkey open-ocpp_ca.key -out open-ocpp_ca.crt
echo ""

echo "Generating private key for Central System..."
openssl ecparam -name prime256v1 -out open-ocpp_central-system.param
openssl ecparam -in open-ocpp_central-system.param -genkey -noout -out open-ocpp_central-system.key
echo "Generating certificate request for Central System..."
openssl req -new -sha256 -key open-ocpp_central-system.key -extensions v3_ca -config open-ocpp_central-system.cnf -out open-ocpp_central-system.csr
echo "Signing Central System certificate with CA certificate..."
openssl x509 -req -sha256 -days 3650 -in open-ocpp_central-system.csr -extensions v3_ca -extfile open-ocpp_central-system.cnf -CA open-ocpp_ca.crt -CAkey open-ocpp_ca.key -CAcreateserial -out open-ocpp_central-system.crt
echo "Verify certificate chain..."
openssl verify -verbose -CAfile open-ocpp_ca.crt open-ocpp_central-system.crt
echo ""

echo "Generating private key for Charge Point..."
openssl ecparam -name prime256v1 -out open-ocpp_charge-point.param
openssl ecparam -in open-ocpp_charge-point.param -genkey -noout -out open-ocpp_charge-point.key
echo "Generating certificate request for Charge Point..."
openssl req -new -sha256 -key open-ocpp_charge-point.key -extensions v3_ca -config open-ocpp_charge-point.cnf -out open-ocpp_charge-point.csr
echo "Signing Charge Point certificate with CA certificate..."
openssl x509 -req -sha256 -days 3650 -in open-ocpp_charge-point.csr -extensions v3_ca -extfile open-ocpp_charge-point.cnf -CA open-ocpp_ca.crt -CAkey open-ocpp_ca.key -CAcreateserial -out open-ocpp_charge-point.crt
echo "Verify certificate chain..."
openssl verify -verbose -CAfile open-ocpp_ca.crt open-ocpp_charge-point.crt
echo ""
