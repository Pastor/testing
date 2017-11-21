package main

import (
	"bytes"
	"fmt"
	"mime/multipart"
	"net/http"
	"net/http/cookiejar"
	"net/textproto"
	"os"
	"strconv"
	"testing"

	"github.com/stretchr/testify/assert"
)

var ctx = LoadConfigurationFromFile("config/v1.yml").Context

func TestMain(m *testing.M) {
	var server = start(ctx)
	server.SetKeepAlivesEnabled(true)
	go func() {
		server.ListenAndServe()
	}()
	var ret = m.Run()
	server.Close()
	os.Exit(ret)
}

func TestMethods(t *testing.T) {
	t.Run("extract", func(t *testing.T) {
		t.Run("success", ExtractSuccess)
		t.Run("405_PUT", ExtractMethodNotAllowedPut)
		t.Run("405_GET", ExtractMethodNotAllowedGet)
		t.Run("405_OPTION", ExtractMethodNotAllowedOption)
		t.Run("405_DELETE", ExtractMethodNotAllowedDelete)
		t.Run("BPE-002001", ExtractIllegalContentType)
		t.Run("BPE-002003", ExtractEmptyContent)
	})
	t.Run("compare", func(t *testing.T) {
		t.Run("405_PUT", CompareMethodNotAllowedPut)
		t.Run("405_GET", CompareMethodNotAllowedGet)
		t.Run("405_OPTION", CompareMethodNotAllowedOption)
		t.Run("405_DELETE", CompareMethodNotAllowedDelete)
	})
}

func CreatePart(contentType string, content []byte) (*bytes.Buffer, string) {
	buffer := &bytes.Buffer{}
	writer := multipart.NewWriter(buffer)
	mimeHeaders := make(textproto.MIMEHeader)
	mimeHeaders.Set(ContentTypeHeader, contentType)
	mimeHeaders.Set(ContentLengthHeader, strconv.Itoa(len(content)))
	part, _ := writer.CreatePart(mimeHeaders)
	part.Write(content)
	return buffer, writer.FormDataContentType()
}

func BuildRequest(method string, query string, contentType string, content []byte) *http.Request {
	if content == nil {
		request, _ := http.NewRequest(method, query, nil)
		return request
	}
	buffer, contentType := CreatePart(contentType, content)
	request, _ := http.NewRequest(method, query, buffer)
	request.Header.Set(ContentTypeHeader, contentType)
	return request
}

func CreateRequest(t *testing.T, method string, path string, contentType string, content []byte, code int, responseContentType string) (*http.Response, error) {
	query := "http://" + ctx.Address + ":" + ctx.Port + fmt.Sprintf("/%s/%s", ctx.toPath(), path)
	cookieJar, _ := cookiejar.New(nil)
	client := &http.Client{Jar: cookieJar, CheckRedirect: func(req *http.Request, via []*http.Request) error {
		return http.ErrUseLastResponse
	}}
	request := BuildRequest(method, query, contentType, content)
	response, err := client.Do(request)
	assert.Nil(t, err, "Ошибка формирования запроса к серверу")
	assert.Equal(t, response.StatusCode, code, fmt.Sprintf("Ошибка запроса %s", response))
	assert.Equal(t,
		response.Header.Get(ContentTypeHeader),
		responseContentType, "Возвращаемый тип не "+responseContentType)
	return response, err
}

func VerifyErrorCode(t *testing.T, response *http.Response, actual ErrorType) {
	exception := UnmarshalException(response)
	assert.Equal(t, exception.Code, actual.Exception.Code,
		fmt.Sprintf("Не верный код ошибки. Ожидали %s пришло %s", actual.Exception.Code, exception.Code))

}
