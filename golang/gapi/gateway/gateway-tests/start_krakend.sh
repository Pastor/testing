#!/bin/sh

export FC_ENABLE=1 \
export FC_SETTINGS="$PWD/config/settings"
export FC_PARTIALS="$PWD/config/partials"
export FC_TEMPLATES="$PWD/config/templates"
export FC_OUT=out.json

~/go/src/krakend-ce/krakend run -d -c "$PWD/config/krakend.json"