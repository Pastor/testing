package main

import (
	"encoding/json"
	"errors"
	"fmt"
	"github.com/gookit/color"
	"io/ioutil"
	"net/http"
	"net/url"
	"os"
	"strconv"
	"strings"
)

const (
	BaseInternalUrl = "10.50.3.159"
	BaseExternalUrl = "keycloak.ein.su:8080"
	BaseUrl         = BaseExternalUrl
	ExternalSecret  = "bf2c382e-7758-4dbc-8059-10ad1fbc1636"
	InternalSecret  = "b7417d53-6a9a-443f-8ecd-e507caf28153"
	ClientId        = "test"
	ClientSecret    = ExternalSecret
	TokenUrl        = "http://" + BaseUrl + "/auth/realms/test/protocol/openid-connect/token"
	UserInfoUrl     = "http://" + BaseUrl + "/auth/realms/test/protocol/openid-connect/userinfo"

	LocalBaseUrl    = "http://127.0.0.1:8000/api/v1"
	InternalBaseUrl = "http://10.50.3.172:8100/api/v1"
	ExternalBaseUrl = "http://10.50.3.172:8100/api/v1"
	ApiBaseUrl      = LocalBaseUrl
)

type Token struct {
	AccessToken      string `json:"access_token"`
	RefreshToken     string `json:"refresh_token"`
	ExpiresIn        int    `json:"expires_in"`
	RefreshExpiresIn int    `json:"refresh_expires_in"`
}

type AuthorizedClient struct {
	Token  *Token
	Client *http.Client
}

type ApiMethod struct {
	StatusCode  int                 `json:"status_code"`
	Description string              `json:"description"`
	Method      string              `json:"method"`
	Path        string              `json:"path"`
	PathParams  []string            `json:"path_params"`
	QueryParams map[string][]string `json:"query_params"`
}

var Methods = []ApiMethod{
	{StatusCode: 404, Method: "GET", Path: "/complexes"},
	{StatusCode: 200, Method: "GET", Path: "/developers"},
	{StatusCode: 401, Method: "GET", Path: "/developers", PathParams: []string{"0"}},
}

func UserInfo(client *AuthorizedClient) (map[string]interface{}, error) {
	request, e := http.NewRequest("GET", UserInfoUrl, nil)
	if e != nil {
		return nil, e
	}
	resp, e := client.Do(request)
	if e != nil {
		return nil, e
	}
	if resp.StatusCode == 200 {
		var entity map[string]interface{}
		bytes, _ := ioutil.ReadAll(resp.Body)
		if err := json.Unmarshal(bytes, &entity); err != nil {
			return nil, err
		}
		return entity, nil
	}
	return nil, errors.New(fmt.Sprintf("Ошибка. Код %s", resp.Status))
}

func GetToken() (*Token, error) {
	data := url.Values{}
	data.Set("grant_type", "client_credentials")
	data.Set("client_id", ClientId)
	data.Set("client_secret", ClientSecret)
	data.Set("scope", "email profile")
	client := &http.Client{}
	encode := data.Encode()
	r, _ := http.NewRequest("POST", TokenUrl, strings.NewReader(encode))
	r.Header.Add("Content-Type", "application/x-www-form-urlencoded")
	r.Header.Add("Content-Length", strconv.Itoa(len(encode)))
	resp, _ := client.Do(r)
	if resp == nil || resp.Body == nil {
		if resp == nil {
			return nil, errors.New("Пустой ответ")
		}
		return nil, errors.New(fmt.Sprintf("Пустое тело. Код %s", resp.Status))
	}
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

func (method ApiMethod) GetPath() string {
	var pathParams = ""

	if method.PathParams != nil {
		pathParams = "/" + strings.Join(method.PathParams, "/")
	}
	return ApiBaseUrl + method.Path + pathParams
}

func main() {
	token, err := GetToken()
	if err != nil {
		color.Error.Println(err)
		os.Exit(-1)
	}
	//parts := strings.Split(token.AccessToken, ".")
	//bytes, err := b64.RawStdEncoding.DecodeString(parts[1])
	//if err != nil {
	//	color.Error.Prompt(err.Error())
	//	os.Exit(-1)
	//}
	//color.Info.Prompt(string(bytes))

	client := &AuthorizedClient{Token: token, Client: http.DefaultClient}

	for _, method := range Methods {
		path := method.GetPath()
		r, _ := http.NewRequest(method.Method, path, nil)
		resp, err := client.Do(r)
		pref := fmt.Sprintf("%s %s", method.Method, path)
		if err != nil {
			color.Error.Prompt(err.Error())
		} else {
			if resp.StatusCode == method.StatusCode {
				color.Debug.Prompt(fmt.Sprintf("%s - %s", pref, resp.Status))
			} else {
				color.Error.Prompt(fmt.Sprintf("%s - %s", pref, resp.Status))
			}
		}
	}
	//
	//info, err := UserInfo(client)
	//if err != nil {
	//	color.Error.Prompt(err.Error())
	//	os.Exit(-1)
	//}
	//color.Info.Prompt(fmt.Sprintf("%v", info))
}

func (client *AuthorizedClient) Do(request *http.Request) (*http.Response, error) {
	request.Header.Add("Authorization", "Bearer "+client.Token.AccessToken)
	return client.Client.Do(request)
}
