package main

import (
	"encoding/base64"
	"encoding/json"
	"errors"
	"flag"
	"fmt"
	"github.com/gookit/color"
	"io/ioutil"
	"net/http"
	"net/url"
	"os"
	"strconv"
	"strings"
)

var TokenCache = make(map[string]*Token)

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

func (o *OAuth) UserInfo(client *AuthorizedClient) (map[string]interface{}, error) {
	request, e := http.NewRequest("GET", o.UserInfoUrl, nil)
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

func (o *OAuth) GetToken(clientName string, scopes []string) (*Token, error) {
	requestScopes := strings.Join(scopes, " ") + " openid"
	token := TokenCache[requestScopes]
	if token != nil {
		return token, nil
	}
	data := url.Values{}
	data.Set("grant_type", "client_credentials")
	data.Set("client_id", o.Clients[clientName].ClientId)
	data.Set("client_secret", o.Clients[clientName].Secret)
	data.Set("scope", requestScopes)
	client := &http.Client{}
	encode := data.Encode()
	r, _ := http.NewRequest("POST", o.Url+"/"+o.TokenUrl, strings.NewReader(encode))
	r.Header.Add("Content-Type", "application/x-www-form-urlencoded")
	r.Header.Add("Content-Length", strconv.Itoa(len(encode)))
	resp, _ := client.Do(r)
	if resp == nil || resp.Body == nil {
		if resp == nil {
			return nil, errors.New("empty response")
		}
		return nil, errors.New(fmt.Sprintf("Пустое тело. Код %s", resp.Status))
	}
	bytes, _ := ioutil.ReadAll(resp.Body)
	if resp.StatusCode == 200 {
		var token Token
		if err := json.Unmarshal(bytes, &token); err != nil {
			return nil, err
		}
		TokenCache[requestScopes] = &token
		return &token, nil
	} else {
		return nil, errors.New(string(bytes))
	}
}

func (m ApiMethod) GetPath(prepareUrl string, properties map[string]string) string {
	for k := range properties {
		prepareUrl = strings.ReplaceAll(prepareUrl, "{"+k+"}", properties[k])
	}
	return prepareUrl
}

func Assert(err error) {
	if err != nil {
		color.Error.Prompt(fmt.Sprintf("%v", err))
		os.Exit(-1)
	}
}

func main() {
	var configurationFile string
	var clientName string
	var gatewayName string
	var showScopes bool
	var c Configuration

	flag.StringVar(&configurationFile, "conf", "eisgs.hoco.api.json", "Файл проверки GatewayAPI")
	flag.StringVar(&clientName, "client", "vtb.crm", "Имя клиента для проверки")
	flag.StringVar(&gatewayName, "gateway", "krakend", "Имя gatewayapi")
	flag.BoolVar(&showScopes, "show_scopes", true, "Показывать скопы полученного токена")
	flag.Parse()

	content, err := ioutil.ReadFile(configurationFile)
	Assert(err)
	err = json.Unmarshal(content, &c)
	Assert(err)

	for k := range c.OAuth.Clients {
		color.Debug.Prompt(fmt.Sprintf("Проверка пользователя %s", k))
		for m := range c.Api.Methods {
			scopes := ""
			method := c.Api.Methods[m]
			token, err := c.OAuth.GetToken(k, method.Scopes)
			if err != nil {
				color.Error.Prompt(fmt.Sprintf("Не удалось получить токен: %v", err))
				continue
			}
			{
				bytes, err := base64.RawStdEncoding.DecodeString(strings.Split(token.AccessToken, ".")[1])
				if err == nil {
					var rawToken map[string]interface{}
					err = json.Unmarshal(bytes, &rawToken)
					if err == nil {
						scopes = rawToken["scope"].(string)
					}
				}
			}
			path := c.Gateway["krakend"].Url + method.GetPath(m, c.Api.Properties)
			authClient := &AuthorizedClient{Token: token, Client: http.DefaultClient}
			r, _ := http.NewRequest(method.Method, path, nil)
			resp, err := authClient.Do(r)
			pref := fmt.Sprintf("%s %s [%s]", method.Method, path, scopes)
			if err != nil {
				color.Error.Prompt(err.Error())
			} else {
				if resp.StatusCode == method.Required.HttpCode {
					color.Debug.Prompt(fmt.Sprintf("%s - %s", pref, resp.Status))
				} else {
					color.Error.Prompt(fmt.Sprintf("%s - %s", pref, resp.Status))
				}
			}
		}
	}
}

func (client *AuthorizedClient) Do(request *http.Request) (*http.Response, error) {
	request.Header.Add("Authorization", "Bearer "+client.Token.AccessToken)
	return client.Client.Do(request)
}
