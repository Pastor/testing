package main

import (
	"context"
	"errors"
	"fmt"
	"net/http"
)

const namespace = "github.com/Pastor/testing/golang/gapi"

var HandlerRegisterer = registrable("gapi")

type registrable string
type ScopeConfiguration struct {
	AllowedScopes []string
	Name          string
}

func (r registrable) RegisterHandlers(f func(
	name string,
	handler func(context.Context, map[string]interface{}, http.Handler) (http.Handler, error),
)) {
	f(string(r), func(ctx context.Context, extra map[string]interface{}, handler http.Handler) (http.Handler, error) {
		cfg := parse(extra)
		if cfg == nil {
			return nil, errors.New("wrong config")
		}
		if cfg.Name != string(r) {
			return nil, fmt.Errorf("unknown register %s", cfg.Name)
		}
		return handler, nil
	})
}

func main() {}

func parse(extra map[string]interface{}) *ScopeConfiguration {
	name, ok := extra["name"].(string)
	if !ok {
		return nil
	}
	rawScopes, ok := extra["scopes"]
	if !ok {
		return nil
	}
	es, ok := rawScopes.([]interface{})
	if !ok || len(es) < 2 {
		return nil
	}
	scopes := make([]string, len(es))
	for i, e := range es {
		scopes[i] = e.(string)
	}

	return &ScopeConfiguration{
		AllowedScopes: scopes,
		Name:          name,
	}
}

//CEL:  && JWT.scope.matches('*access*')
