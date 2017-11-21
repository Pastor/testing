package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"time"

	"encoding/json"
	"strconv"
	"strings"

	"github.com/gorilla/mux"
)

type Context struct {
	Dir      string `json:"directory"`
	Vendor   string `json:"vendor"`
	Modality string `json:"modality"`
	Version  string `json:"version"`
	Port     string `json:"port"`
	BasePath string `json:"base_path"`
	Address  string `json:"address"`

	ContentRange  RangeInt `json:"content_range"`
	TemplateRange RangeInt `json:"template_range"`
}

type RangeInt struct {
	Min int `json:"min"`
	Max int `json:"max"`
}

func (ctx Context) toPath() string {
	return ctx.Vendor + "/" + ctx.Modality + "/" + ctx.Version + "/" + ctx.BasePath
}

func (ctx Context) build() Context {
	flag.StringVar(&ctx.Vendor, "vendor", "grcc", "Наименование вендора")
	flag.StringVar(&ctx.Modality, "modality", "photo", "Модальность")
	flag.StringVar(&ctx.Version, "version", "1.0.0", "Версия нейронной сети")
	flag.StringVar(&ctx.Port, "port", "8081", "Порт сервера")
	flag.StringVar(&ctx.BasePath, "base_path", "unknown", "Базовый путь к методам")
	flag.StringVar(&ctx.Address, "address", "127.0.0.1", "Адрес на который цепляется сервер")
	flag.Parse()
	return ctx;
}

func WriteError(w http.ResponseWriter, code string, message string) {
	WriteException(w, Exception{Code: code, Message: message})
}

func WriteException(w http.ResponseWriter, ex Exception) {
	w.Header().Set(ContentTypeHeader, JsonContent+CharsetUtf8)
	w.WriteHeader(http.StatusBadRequest)
	content, _ := json.Marshal(ex)
	w.Write(content)
	log.Println(string(content))
}

func Extract(w http.ResponseWriter, r *http.Request, ctx *Context) {
	var content [] byte
	reader, err := r.MultipartReader()
	if err != nil {
		WriteException(w, IllegalContentTypeError.Exception)
		return
	}
	for {
		part, err := reader.NextPart()
		if err != nil {
			log.Println(err)
			WriteException(w, CantReadContentError.Exception)
			return
		} else if part == nil {
			WriteException(w, CantReadContentError.Exception)
			return
		}
		partContentType := part.Header.Get(ContentTypeHeader)
		if strings.Compare(ImageJpegContent, partContentType) != 0 &&
			strings.Compare(SoundPcmContent, partContentType) != 0 {
			WriteException(w, IllegalContentTypeError.Exception)
			return
		}
		//log.Printf("[Part] Name: {%s}", part.FormName())
		//log.Printf("[Part] File: {%s}", part.FileName())
		//log.Printf("[Part] Headers: {%s}", part.Header)
		contentLength, _ := strconv.Atoi(part.Header.Get(ContentLengthHeader))
		content = make([]byte, contentLength)
		part.Read(content)
		break
	}
	defer r.Body.Close()
	if err != nil {
		log.Println(fmt.Sprintf("Ошибка чтения биометрического образца %s", err))
		WriteException(w, CantReadContentError.Exception)
		return
	}
	contentLength := len(content)
	if contentLength > ctx.ContentRange.Max ||
		contentLength < ctx.ContentRange.Min {
		log.Println(fmt.Sprintf("Размер биометрического образца не соответствует требованиям %d (%d, %d)",
			len(content), ctx.ContentRange.Min, ctx.ContentRange.Max))
		WriteException(w, CantReadContentError.Exception)
		return
	}
	w.WriteHeader(http.StatusOK)
	w.Header().Set(ContentTypeHeader, OctetStreamContent)
	w.Write(make([]byte, 256))
}

func Compare(w http.ResponseWriter, r *http.Request, ctx *Context) {
	var content [] byte
	reader, err := r.MultipartReader()
	if err != nil {
		WriteException(w, IllegalContentTypeError.Exception)
		return
	}
	for {
		part, err := reader.NextPart()
		if err != nil {
			log.Println(fmt.Sprintf("Ошибка чтения данных из потока %s", err))
			WriteException(w, CantReadContentError.Exception)
			return
		} else if part == nil {
			log.Println(fmt.Sprintf("Не удалось считать part %s", err))
			WriteException(w, CantReadContentError.Exception)
			return
		}
		partContentType := part.Header.Get(ContentTypeHeader)
		if strings.Compare(OctetStreamContent, partContentType) != 0 {
			WriteException(w, IllegalContentTypeError.Exception)
			log.Println(fmt.Sprintf("Тип данных не соответствует ни одному типу принимаемому методом %s", partContentType))
			return
		}
		contentLength, _ := strconv.Atoi(part.Header.Get(ContentLengthHeader))
		content = make([]byte, contentLength)
		part.Read(content)
		break
	}
	defer r.Body.Close()
	if err != nil {
		log.Println(fmt.Sprintf("Ошибка чтения биометрического образца %s", err))
		WriteException(w, CantReadContentError.Exception)
		return
	}
	contentLength := len(content)
	if contentLength > ctx.ContentRange.Max ||
		contentLength < ctx.ContentRange.Min {
		log.Println(fmt.Sprintf("Размер биометрического образца не соответствует требованиям %d (%d, %d)",
			len(content), ctx.ContentRange.Min, ctx.ContentRange.Max))
		WriteException(w, CantReadContentError.Exception)
		return
	}
	w.WriteHeader(http.StatusOK)
	w.Header().Set(ContentTypeHeader, JsonContent+CharsetUtf8)
	jsonContent, _ := json.Marshal(CompareResult{Score: "0.8"})
	w.Write(jsonContent)
}

func WrappedRoute(f func(http.ResponseWriter, *http.Request, *Context), ctx *Context) func(http.ResponseWriter, *http.Request) {
	return func(w http.ResponseWriter, r *http.Request) {
		f(w, r, ctx)
	}
}

func start(ctx Context) *http.Server {
	path := ctx.toPath();
	router := mux.NewRouter().StrictSlash(true)
	router.HandleFunc(fmt.Sprintf("/%s/%s", path, PATTERN_EXTRACT),
		WrappedRoute(Extract, &ctx)).Methods("POST")
	router.HandleFunc(fmt.Sprintf("/%s/%s", path, PATTERN_COMPARE),
		WrappedRoute(Compare, &ctx))
	router.PathPrefix("/static/").Handler(http.StripPrefix("/static/", http.FileServer(http.Dir(ctx.Dir))))

	//log.Println("Extract http://" + ctx.address + ":" + ctx.port + fmt.Sprintf("/%s/%s", path, PATTERN_EXTRACT))
	//log.Println("Compare http://" + ctx.address + ":" + ctx.port + fmt.Sprintf("/%s/%s", path, PATTERN_COMPARE))
	return &http.Server{
		Handler:      router,
		Addr:         "0.0.0.0:" + ctx.Port,
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}
}
