package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
)

const ContentTypeHeader = "Content-Type"
const ContentLengthHeader = "Content-Length"
const ImageJpegContent = "image/jpeg"
const SoundPcmContent = "audio/pcm"
const OctetStreamContent = "application/octet-stream"
const JsonContent = "application/json"
const TextPlainContent = "text/plain"
const CharsetUtf8 = "; charset=utf-8"

const PATTERN_EXTRACT = "pattern/extract"
const PATTERN_COMPARE = "pattern/compare"

type CompareResult struct {
	Score string `json:"score"`
}

func UnmarshalException(response *http.Response) Exception {
	var ex Exception;
	content, _ := ioutil.ReadAll(response.Body)
	defer response.Body.Close()
	err := json.Unmarshal(content, &ex)
	if err != nil {
		log.Println(fmt.Sprintf("Ошибка разбора результата {%s}. [%s]", string(content), err))
	}
	return ex
}
