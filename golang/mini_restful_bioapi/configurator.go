package main

import (
	"io/ioutil"
	"log"

	"github.com/ghodss/yaml"
	"github.com/go-redis/redis"
)

type Configuration struct {
	Schema     string       `json:"schema"`
	Generation string       `json:"generation"`
	Context    Context      `json:"context"`
	Cache      CacheContext `json:"cache"`
}

type CacheContext struct {
	Address string `json:"address"`
}

type Cache struct {
	Client *redis.Client
}

func (cache *Cache) Connect(context CacheContext) error {
	cache.Client = redis.NewClient(&redis.Options{
		Addr:     context.Address,
		Password: "",
		DB:       0,
	})
	return cache.Client.Ping().Err()
}

func (cache *Cache) Put(key string, value string) error {
	return cache.Client.Set(key, value, 0).Err();
}

func (cache *Cache) Get(key string) (string, error) {
	return cache.Client.Get(key).Result()
}

func (cache *Cache) Close() {
	cache.Client.Close()
}

func (cache *Cache) Exists(key string) bool {
	val, err := cache.Client.Get(key).Result()
	if err == redis.Nil {
		return false
	} else if err != nil {
		return false
	}
	return len(val) != 0
}

func LoadConfiguration(content string) Configuration {
	var configuration = Configuration{}
	err := yaml.Unmarshal([]byte(content), &configuration)
	if err != nil {
		log.Printf("Ошибка разбора: %s", err)
	}
	return configuration
}

func LoadConfigurationFromFile(filename string) Configuration {
	content, _ := ioutil.ReadFile(filename)
	return LoadConfiguration(string(content))
}

func StoreConfiguration(configuration Configuration) string {
	content, err := yaml.Marshal(configuration)
	if err != nil {
		log.Println(err)
	}
	return string(content)
}
