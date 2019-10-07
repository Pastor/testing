package main

import (
	b64 "encoding/base64"
	"encoding/json"
	"errors"
	"fmt"
	"io/ioutil"
	"net/http"
	"net/url"
	"strconv"
	"strings"
)

const (
	ClientId     = "test"
	ClientSecret = "bf2c382e-7758-4dbc-8059-10ad1fbc1636"
	TokenUrl     = "http://keycloak.ein.su:8080/auth/realms/test/protocol/openid-connect/token"
)

type Token struct {
	AccessToken      string `json:"access_token"`
	RefreshToken     string `json:"refresh_token"`
	ExpiresIn        int    `json:"expires_in"`
	RefreshExpiresIn int    `json:"refresh_expires_in"`
}

func GetToken() (*Token, error) {
	data := url.Values{}
	data.Set("grant_type", "client_credentials")
	data.Set("client_id", ClientId)
	data.Set("client_secret", ClientSecret)
	data.Set("scope", "email")
	client := &http.Client{}
	encode := data.Encode()
	r, _ := http.NewRequest("POST", TokenUrl, strings.NewReader(encode))
	r.Header.Add("Content-Type", "application/x-www-form-urlencoded")
	r.Header.Add("Content-Length", strconv.Itoa(len(encode)))
	resp, _ := client.Do(r)
	bytes, _ := ioutil.ReadAll(resp.Body)
	if resp.StatusCode == 200 {
		var token Token
		if err := json.Unmarshal(bytes, &token); err != nil {
			return nil, err
		}
		return &token, nil
	} else {
		return nil, errors.New(string(bytes))
	}
}

func main() {
	token, err := GetToken()
	if err != nil {
		panic(err)
	}
	parts := strings.Split(token.AccessToken, ".")
	bytes, err := b64.RawStdEncoding.DecodeString(parts[1])
	if err != nil {
		panic(err)
	}
	fmt.Print(string(bytes))
}
