package main

import (
	"encoding/json"
	"fmt"
	"github.com/devopsfaith/krakend/config"
	"github.com/devopsfaith/krakend/plugin"
	"github.com/google/cel-go/cel"
	"github.com/google/cel-go/checker/decls"
	"log"
	"time"
)

const pluginName = "krakend-scopes"
const namespace = "github.com/Pastor/testing/golang/gapi"

func main() {
	var jwt map[string]interface{}

	err := json.Unmarshal([]byte(TestingJWT), &jwt)
	if err != nil {
		log.Panic(err)
	}
	Evaluate("has(JWT.clientId) && JWT.scope.matches('.*access.*')", jwt)
	Evaluate("has(JWT.clientId) && JWT.scope.matches('.*test.*')", jwt)
	Evaluate("has(JWT.clientId) && JWT.scope.matches('.*developers.read.extended.*')", jwt)
	Evaluate("JWT.clientId == 'test'", jwt)
}

func CheckLoadingPlugin() {
	register := plugin.NewRegister()

	fmt.Println(plugin.Load(config.Plugin{
		Folder:  "/home/pastor/repo/testing/golang/gapi/",
		Pattern: ".so",
	}, register))
	CheckExternal(register)
}

func CheckExternal(register *plugin.Register) {
	n1Register, ok := register.External.Get(namespace)
	if !ok {
		fmt.Printf("%s not registered\n", namespace)
		return
	}
	v, ok := n1Register.Get(pluginName)
	if !ok {
		fmt.Printf("%s not registered into %s\n", pluginName, namespace)
		return
	}
	_, ok = v.(func(int) int)
	if !ok {
		fmt.Printf("unexpected registered component into %s %v", namespace, v)
		return
	}
}

func Evaluate(text string, data map[string]interface{}) {
	parameters := map[string]interface{}{
		JwtKey: data,
		NowKey: time.Now().Format(time.RFC3339),
	}
	eval, _ := CreateProgram(text)
	res, _, err := eval.Eval(parameters)
	if err != nil {
		log.Fatal(err)
	}
	v, ok := res.Value().(bool)
	if ok {
		var reason string
		if v {
			reason = "success"
		} else {
			reason = "failure"
		}
		log.Println(text, "evaluate result", reason)
	} else {
		log.Fatal(err)
	}
}

func CreateProgram(text string) (cel.Program, error) {
	env, err := cel.NewEnv(DefaultDeclarations())
	if err != nil {
		log.Fatal(err)
	}
	ast, iss := env.Parse(text)
	if iss != nil && iss.Err() != nil {
		log.Fatal(iss.Err())
	}
	c, iss := env.Check(ast)
	if iss != nil && iss.Err() != nil {
		log.Fatal(iss.Err())
	}
	return env.Program(c)
}

const (
	PreKey  = "req"
	PostKey = "resp"
	JwtKey  = "JWT"
	NowKey  = "now"
)

func DefaultDeclarations() cel.EnvOption {
	return cel.Declarations(
		decls.NewIdent(NowKey, decls.String, nil),

		decls.NewIdent(PreKey+"_method", decls.String, nil),
		decls.NewIdent(PreKey+"_path", decls.String, nil),
		decls.NewIdent(PreKey+"_params", decls.NewMapType(decls.String, decls.String), nil),
		decls.NewIdent(PreKey+"_headers", decls.NewMapType(decls.String, decls.NewListType(decls.String)), nil),
		decls.NewIdent(PreKey+"_querystring", decls.NewMapType(decls.String, decls.NewListType(decls.String)), nil),

		decls.NewIdent(PostKey+"_completed", decls.Bool, nil),
		decls.NewIdent(PostKey+"_metadata_status", decls.Int, nil),
		decls.NewIdent(PostKey+"_metadata_headers", decls.NewMapType(decls.String, decls.NewListType(decls.String)), nil),
		decls.NewIdent(PostKey+"_data", decls.NewMapType(decls.String, decls.Dyn), nil),

		decls.NewIdent(JwtKey, decls.NewMapType(decls.String, decls.Dyn), nil),
	)
}

const TestingJWT string = `{
  "jti": "f86c1180-914b-430a-a372-a1c163794a4f",
  "exp": 1570391034,
  "nbf": 0,
  "iat": 1570390734,
  "iss": "http://keycloak.ein.su:8080/auth/realms/test",
  "aud": "account",
  "sub": "ec04275a-9c85-477f-b266-e9f7c8fefb7e",
  "typ": "Bearer",
  "azp": "test",
  "auth_time": 0,
  "session_state": "4ee1f456-a10e-4ecd-a16e-5da24ac7051e",
  "acr": "1",
  "realm_access": {
    "roles": [
      "offline_access",
      "uma_authorization"
    ]
  },
  "resource_access": {
    "test": {
      "roles": [
        "uma_protection"
      ]
    },
    "account": {
      "roles": [
        "manage-account",
        "manage-account-links",
        "view-profile"
      ]
    }
  },
  "scope": "profile test developers.read.extended email ",
  "email_verified": false,
  "clientHost": "109.252.76.164",
  "clientId": "test",
  "preferred_username": "service-account-test",
  "clientAddress": "109.252.76.164",
  "email": "service-account-test@placeholder.org"
}`
