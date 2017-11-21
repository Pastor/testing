package main

import (
	"fmt"
	"testing"
	"time"

	"github.com/coreos/etcd/clientv3"
	"github.com/stretchr/testify/assert"
)

func TestController_Connect(t *testing.T) {
	client, err := clientv3.New(clientv3.Config{
		Endpoints:   []string{"localhost:2379"},
		DialTimeout: 5 * time.Second,
	})
	assert.Nil(t, err, fmt.Sprintf("Ошибка подключения к etcd: %s", err.Error()))
	if client != nil {
		client.Close()
	}
}
