package main

import (
	"net/http"
	"testing"
)

func CompareMethodNotAllowedPut(t *testing.T) {
	response, _ := CreateRequest(t, "PUT", PATTERN_EXTRACT,
		ImageJpegContent, make([] byte, 100), http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}

func CompareMethodNotAllowedGet(t *testing.T) {
	response, _ := CreateRequest(t, "GET", PATTERN_EXTRACT,
		ImageJpegContent, nil, http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}

func CompareMethodNotAllowedDelete(t *testing.T) {
	response, _ := CreateRequest(t, "DELETE", PATTERN_EXTRACT,
		ImageJpegContent, nil, http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}

func CompareMethodNotAllowedOption(t *testing.T) {
	response, _ := CreateRequest(t, "OPTION", PATTERN_EXTRACT,
		ImageJpegContent, nil, http.StatusMethodNotAllowed, TextPlainContent+CharsetUtf8)
	defer response.Body.Close()
}
