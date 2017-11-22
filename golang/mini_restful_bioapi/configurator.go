package main

import (
	"io/ioutil"
	"log"

	"github.com/ghodss/yaml"
	"github.com/coreos/etcd/clientv3"
	"time"
	"context"
)

type Configuration struct {
	Schema       string              `json:"schema"`
	Generation   string              `json:"generation"`
	Context      Context             `json:"context"`
	Configurator ConfiguratorContext `json:"configurator"`
}

type ConfiguratorContext struct {
	Addresses     []string `json:"addresses"`
	PutKeyTimeout int      `json:"put_key_timeout"`
	GetKeyTimeout int      `json:"get_key_timeout"`
}

type Configurator struct {
	Client        *clientv3.Client
	Context       context.Context
	Configuration ConfiguratorContext
}

func NewConfigurator(configuration Configuration) (Configurator, error) {
	client, err := clientv3.New(clientv3.Config{
		Endpoints:   configuration.Configurator.Addresses,
		DialTimeout: 5 * time.Second,
	})
	return Configurator{
		Client:        client,
		Context:       context.Background(),
		Configuration: configuration.Configurator,
	}, err
}

func (c *Configurator) Close() {
	if c.Client != nil {
		c.Client.Close()
	}
	c.Client = nil;
}

func (c *Configurator) Put(key string, value string) error {
	ctx, cancel := context.WithTimeout(c.Context, time.Duration(c.Configuration.PutKeyTimeout)*time.Second)
	_, err := c.Client.KV.Put(ctx, key, value)
	cancel()
	return err
}

func (c *Configurator) Get(key string) (string, error) {
	ctx, cancel := context.WithTimeout(c.Context, time.Duration(c.Configuration.GetKeyTimeout)*time.Second)
	response, err := c.Client.KV.Get(ctx, key)
	defer cancel()
	if err != nil {
		return "", err
	} else if len(response.Kvs) == 0 {
		return "", nil
	}
	return string(response.Kvs[0].Value), nil
}

func (c *Configurator) Exists(key string) bool {
	val, err := c.Get(key)
	return len(val) != 0 && err == nil
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
