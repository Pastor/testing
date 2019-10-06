package main

import (
	"fmt"
	jose "github.com/devopsfaith/krakend-jose"
	"io"
	"io/ioutil"
)

const pluginName = "krakend-scopes"

var Registrable registrable

type registrable int

type Mein struct{}

func (m Mein) Reject(map[string]interface{}) bool {
	return true
}

func (r *registrable) RegisterDecoder(setter func(name string, dec func(bool) func(io.Reader, *map[string]interface{}) error) error) error {
	fmt.Println("registrable", r, "from plugin", pluginName, "is registering its decoder components")

	return setter(pluginName, decoderFactory)
}

func (r *registrable) RegisterExternal(setter func(namespace, name string, v interface{})) error {
	fmt.Println("registrable", r, "from plugin", pluginName, "is registering its components depending on external modules")

	setter("namespace1", pluginName, doubleInt)
	var reject jose.Rejecter = Mein{}
	fmt.Println(reject)
	return nil
}

func doubleInt(x int) int {
	return 2 * x
}

func decoderFactory(bool) func(reader io.Reader, _ *map[string]interface{}) error {
	fmt.Println("calling the decoder factory:", pluginName)

	return decoder
}

func decoder(reader io.Reader, _ *map[string]interface{}) error {
	fmt.Println("calling the decoder:", pluginName)

	d, err := ioutil.ReadAll(reader)
	if err != nil {
		return err
	}
	fmt.Println("decoder:", pluginName, string(d))
	return nil
}

func main() {}
