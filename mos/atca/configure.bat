@echo off

mos config-set sys.atca.enable=true
mos -X atca-set-config atca-aws-test.yaml --dry-run=false
mos -X atca-lock-zone config --dry-run=false
mos -X atca-lock-zone data --dry-run=false
mos -X atca-set-key 4 slot4.key --dry-run=false
mos -X atca-set-key 0 ecc.key.pem --write-key=slot4.key --dry-run=false
mos put ecc.crt.pem
mos config-set http.listen_addr=:443 http.ssl_cert=ecc.crt.pem http.ssl_key=ATCA:0