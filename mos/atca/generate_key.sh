#!/bin/sh
openssl ecparam -out ecc.key.pem -name prime256v1 -genkey
openssl req -new -subj "/C=IE/L=Dublin/O=ACME Ltd/OU=Testing/CN=test.acme.com" -sha256 -key ecc.key.pem -text -out ecc.csr.tmpl
openssl x509 -in ecc.csr.tmpl -text -out ecc.crt.pem -req -signkey ecc.key.pem -days 3650
openssl rand -hex 32 > slot4.key