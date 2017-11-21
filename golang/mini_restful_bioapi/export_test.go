package main

import (
	"testing"
	"io/ioutil"
	"github.com/stretchr/testify/assert"
	"net/http"
)

func ExtractSuccess(t *testing.T) {
	response, err := CreateRequest(t, "POST", PATTERN_EXTRACT,
		ImageJpegContent, make([] byte, 100), http.StatusOK, OctetStreamContent)
	content, err := ioutil.ReadAll(response.Body)
	defer response.Body.Close()
	assert.Nil(t, err, "Ошибка чтения шаблона");
	assert.Equal(t, len(content), 256, "Размер шаблона не верный")
}

func ExtractEmptyContent(t *testing.T) {
	response, _ := CreateRequest(t, "POST", PATTERN_EXTRACT,
		ImageJpegContent, make([] byte, 0), http.StatusBadRequest, JsonContent+CharsetUtf8)
	VerifyErrorCode(t, response, CantReadContentError)
}

func ExtractMethodNotAllowedPut(t *testing.T) {
	response, _ := CreateRequest(t, "PUT", PATTERN_EXTRACT,
		ImageJpegContent, make([] byte, 100), http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}

func ExtractMethodNotAllowedGet(t *testing.T) {
	response, _ := CreateRequest(t, "GET", PATTERN_EXTRACT,
		ImageJpegContent, nil, http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}

func ExtractMethodNotAllowedDelete(t *testing.T) {
	response, _ := CreateRequest(t, "DELETE", PATTERN_EXTRACT,
		ImageJpegContent, nil, http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}

func ExtractMethodNotAllowedOption(t *testing.T) {
	response, _ := CreateRequest(t, "OPTION", PATTERN_EXTRACT,
		ImageJpegContent, nil, http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}

func ExtractIllegalContentType(t *testing.T) {
	response, _ := CreateRequest(t, "POST", PATTERN_EXTRACT,
		OctetStreamContent, make([] byte, 100), http.StatusBadRequest, JsonContent+CharsetUtf8)
	VerifyErrorCode(t, response, IllegalContentTypeError)
}
