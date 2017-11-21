package main

import (
	"flag"
	"fmt"
	"log"
	"net/http"
	"time"

	"github.com/gorilla/mux"
	"encoding/json"
	"strings"
	"strconv"
)

type Context struct {
	dir      string
	vendor   string
	modality string
	version  string
	port     string
	basePath string
	address  string
}

func (ctx Context) toPath() string {
	return ctx.vendor + "/" + ctx.modality + "/" + ctx.version + "/" + ctx.basePath
}

func (ctx Context) build() Context {
	flag.StringVar(&ctx.vendor, "vendor", "grcc", "Наименование вендора")
	flag.StringVar(&ctx.modality, "modality", "photo", "Модальность")
	flag.StringVar(&ctx.version, "version", "1.0.0", "Версия нейронной сети")
	flag.StringVar(&ctx.port, "port", "8081", "Порт сервера")
	flag.StringVar(&ctx.basePath, "base_path", "unknown", "Базовый путь к методам")
	flag.StringVar(&ctx.address, "address", "127.0.0.1", "Адрес на который цепляется сервер")
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

func extract(w http.ResponseWriter, r *http.Request, ctx *Context) {
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
	if len(content) == 0 {
		log.Println(fmt.Sprintf("Размер биометрического образца меньше требуемого %d", len(content)))
		WriteException(w, CantReadContentError.Exception)
		return
	}
	w.WriteHeader(http.StatusOK)
	w.Header().Set(ContentTypeHeader, OctetStreamContent)
	w.Write(make([]byte, 256))
}

func compare(w http.ResponseWriter, r *http.Request, ctx *Context) {
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
	if len(content) == 0 {
		log.Println(fmt.Sprintf("Размер биометрического образца меньше требуемого %d", len(content)))
		WriteException(w, CantReadContentError.Exception)
		return
	}
	w.WriteHeader(http.StatusOK)
	w.Header().Set(ContentTypeHeader, JsonContent+CharsetUtf8)
	jsonContent, _ := json.Marshal(CompareResult{Score: "0.8"})
	w.Write(jsonContent)
}

func wrapped_route(f func(http.ResponseWriter, *http.Request, *Context), ctx *Context) func(http.ResponseWriter, *http.Request) {
	return func(w http.ResponseWriter, r *http.Request) {
		f(w, r, ctx)
	}
}

func start(ctx Context) *http.Server {
	path := ctx.toPath();
	router := mux.NewRouter().StrictSlash(true)
	router.HandleFunc(fmt.Sprintf("/%s/%s", path, PATTERN_EXTRACT), wrapped_route(extract, &ctx)).Methods("POST")
	router.HandleFunc(fmt.Sprintf("/%s/%s", path, PATTERN_COMPARE), wrapped_route(compare, &ctx))
	router.PathPrefix("/static/").Handler(http.StripPrefix("/static/", http.FileServer(http.Dir(ctx.dir))))

	//log.Println("Extract http://" + ctx.address + ":" + ctx.port + fmt.Sprintf("/%s/%s", path, PATTERN_EXTRACT))
	//log.Println("Compare http://" + ctx.address + ":" + ctx.port + fmt.Sprintf("/%s/%s", path, PATTERN_COMPARE))
	return &http.Server{
		Handler:      router,
		Addr:         "0.0.0.0:" + ctx.port,
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}
}

func main() {
	var server = start(Context{}.build())
	server.ListenAndServe()
}
