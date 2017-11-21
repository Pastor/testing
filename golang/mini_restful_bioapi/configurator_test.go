package main

import (
	"io/ioutil"
	"testing"

	"github.com/satori/go.uuid"
	"github.com/stretchr/testify/assert"
)

const VENDOR = "grcc"
const VERSION = "1.0.0"
const CONFIGURATION = "config/v1.yml"

func TestConfiguration_LoadFromFile(t *testing.T) {
	var created = Configuration{
		Schema:     "v1",
		Generation: "v32",
		Cache: CacheContext{
			Address: "127.0.0.1:6379",
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

func TestConfiguration_StoreConfiguration(t *testing.T) {
	var created = Configuration{
		Schema:     "v1",
		Generation: "v32",
		Cache: CacheContext{
			Address: "127.0.0.1:6379",
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
	assert.Equal(t, `cache:
  address: 127.0.0.1:6379
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

func TestCache_Connect(t *testing.T) {
	var configuration = LoadConfigurationFromFile(CONFIGURATION)
	var cache = Cache{}
	err := cache.Connect(configuration.Cache)
	assert.Nil(t, err, "Ошибка подключения к серверу кэш")
	defer cache.Close()
}

func TestCache_Put(t *testing.T) {
	var configuration = LoadConfigurationFromFile(CONFIGURATION)
	var cache = Cache{}
	err := cache.Connect(configuration.Cache)
	assert.Nil(t, err, "Ошибка подключения к серверу кэш")
	defer cache.Close()
	var key = uuid.NewV4()
	err = cache.Put(key.String(), "VALUE")
	assert.Nil(t, err, "Ошибка помещения значения")
}

func TestCache_Get(t *testing.T) {
	var configuration = LoadConfigurationFromFile(CONFIGURATION)
	var cache = Cache{}
	err := cache.Connect(configuration.Cache)
	assert.Nil(t, err, "Ошибка подключения к серверу кэш")
	defer cache.Close()
	var key = uuid.NewV4()
	err = cache.Put(key.String(), "VALUE")
	assert.Nil(t, err, "Ошибка помещения значения")
	_, err = cache.Get(key.String())
	assert.Nil(t, err, "Ошибка получения значения")
}
