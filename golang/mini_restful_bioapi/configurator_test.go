package main

import (
	"io/ioutil"
	"testing"

	"github.com/satori/go.uuid"
	"github.com/stretchr/testify/assert"
	"fmt"
)

func ConfigurationLoadFromFile(t *testing.T) {
	var created = Configuration{
		Schema:     "v1",
		Generation: "v32",
		Configurator: ConfiguratorContext{
			Addresses:     []string{"127.0.0.1:2379"},
			PutKeyTimeout: 1,
			GetKeyTimeout: 1,
		},
		Context: Context{
			Address:       "127.0.0.1",
			BasePath:      "unknown",
			Dir:           ".",
			Modality:      "photo",
			Port:          "8081",
			Vendor:        VENDOR,
			Version:       VERSION,
			ContentRange:  RangeInt{Min: 16, Max: 5000000},
			TemplateRange: RangeInt{Min: 16, Max: 260},
		}}
	content, err := ioutil.ReadFile(CONFIGURATION)
	assert.Nil(t, err, "Ошибка открытия файла настроек")
	var loaded = LoadConfiguration(string(content))
	assert.Equal(t, loaded, created, "Не правильная загрузка контекста")
}

func ConfigurationStoreConfiguration(t *testing.T) {
	var created = Configuration{
		Schema:     "v1",
		Generation: "v32",
		Configurator: ConfiguratorContext{
			Addresses:     []string{"127.0.0.1:2379"},
			PutKeyTimeout: 5,
			GetKeyTimeout: 5,
		},
		Context: Context{
			Address:       "127.0.0.1",
			BasePath:      "unknown",
			Dir:           ".",
			Modality:      "photo",
			Port:          "8081",
			Vendor:        VENDOR,
			Version:       VERSION,
			ContentRange:  RangeInt{Min: 16, Max: 5000000},
			TemplateRange: RangeInt{Min: 16, Max: 260},
		}}
	var content = StoreConfiguration(created)
	assert.Equal(t, `configurator:
  addresses:
  - 127.0.0.1:2379
  get_key_timeout: 5
  put_key_timeout: 5
context:
  address: 127.0.0.1
  base_path: unknown
  content_range:
    max: 5000000
    min: 16
  directory: .
  modality: photo
  port: "8081"
  template_range:
    max: 260
    min: 16
  vendor: `+ VENDOR+ `
  version: `+ VERSION+ `
generation: v32
schema: v1
`, content)
}

func ConfiguratorConnect(t *testing.T) {
	configurator, err := NewConfigurator(configuration)
	assert.Nil(t, err, fmt.Sprintf("Ошибка создание конфигуратора %s", TranslateError(err)))
	defer configurator.Close()
}

func ConfiguratorPut(t *testing.T) {
	configurator, err := NewConfigurator(configuration)
	assert.Nil(t, err, fmt.Sprintf("Ошибка создание конфигуратора %s", TranslateError(err)))
	defer configurator.Close()
	var key = uuid.NewV4()
	err = configurator.Put(key.String(), "VALUE")
	assert.Nil(t, err, "Ошибка помещения значения")
}

func ConfiguratorGet(t *testing.T) {
	configurator, err := NewConfigurator(configuration)
	assert.Nil(t, err, fmt.Sprintf("Ошибка создание конфигуратора %s", TranslateError(err)))
	defer configurator.Close()
	var key = uuid.NewV4()
	err = configurator.Put(key.String(), "VALUE")
	assert.Nil(t, err, "Ошибка помещения значения")
	_, err = configurator.Get(key.String())
	assert.Nil(t, err, "Ошибка получения значения")
}
