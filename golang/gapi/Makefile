.PHONY: build

all: build

download:
	go get -u github.com/swaggo/swag/cmd/swag
	go get -u github.com/gin-contrib/logger
	go get -u github.com/gin-gonic/contrib/static
	go get -u github.com/gin-gonic/gin
	go get -u github.com/rs/zerolog
	go get -u github.com/rs/zerolog/log
	go get -u github.com/swaggo/gin-swagger
	go get -u github.com/swaggo/gin-swagger/swaggerFiles
	go get -u github.com/swaggo/swag/example/celler/httputil

build:
	swag init --dir ./api/event-api --output ./api/event-api/docs --parseVendor
	go build ./api/event-api

build-plugin:
	go build -buildmode=plugin -o krakend-scope-plugin.so ./plugin
	chmod a+x krakend-scope-plugin.so

run:
	./event-api &

stop:
	killall event-api