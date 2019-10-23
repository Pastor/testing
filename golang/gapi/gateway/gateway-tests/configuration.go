package main

type OAuthClient struct {
	ClientId string `json:"client_id"`
	Secret   string `json:"secret"`
}

type OAuth struct {
	Url         string                 `json:"url"`
	TokenUrl    string                 `json:"token"`
	UserInfoUrl string                 `json:"user_info"`
	Clients     map[string]OAuthClient `json:"clients"`
}

type Gateway struct {
	Url string `json:"url"`
}

type ApiMethodResponse struct {
	HttpCode int `json:"http_code"`
}

type ApiMethod struct {
	Scopes   []string          `json:"scopes"`
	Method   string            `json:"method"`
	Required ApiMethodResponse `json:"required"`
}

type Api struct {
	Properties map[string]string    `json:"properties"`
	Methods    map[string]ApiMethod `json:"methods"`
}

type Configuration struct {
	OAuth   OAuth              `json:"oauth"`
	Gateway map[string]Gateway `json:"gateway"`
	Api     Api                `json:"api"`
}
