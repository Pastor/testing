package main

import (
	"context"
	"encoding/json"
	"log"
	"math/rand"
	"net/http"
	"os"
	"strconv"

	"github.com/gorilla/handlers"
	"github.com/gorilla/mux"
	"github.com/jackc/pgx"
)

const EnvironmentDatabaseUrl = "DATABASE_URL"
const DefaultContentType = "application/json"
const DefaultConnectionString = "host=127.0.0.1 port=5432 user=postgres password=postgres database=postgres"

type Book struct {
	ID     string  `json:"id"`
	Title  string  `json:"title"`
	Author *Author `json:"author"`
}

type Author struct {
	FirstName string `json:"first_name"`
	LastName  string `json:"last_name"`
}

var books []Book

func getBooks(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", DefaultContentType)
	json.NewEncoder(w).Encode(books)
}

func getBook(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", DefaultContentType)
	params := mux.Vars(r)
	for _, item := range books {
		if item.ID == params["id"] {
			json.NewEncoder(w).Encode(item)
			return
		}
	}
	w.WriteHeader(http.StatusNotFound)
}

func createBook(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", DefaultContentType)
	var book Book
	_ = json.NewDecoder(r.Body).Decode(&book)
	book.ID = strconv.Itoa(rand.Intn(1000000))
	books = append(books, book)
	json.NewEncoder(w).Encode(book)
}

func updateBook(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", DefaultContentType)
	params := mux.Vars(r)
	for index, item := range books {
		if item.ID == params["id"] {
			books = append(books[:index], books[index+1:]...)
			var book Book
			_ = json.NewDecoder(r.Body).Decode(&book)
			book.ID = params["id"]
			books = append(books, book)
			json.NewEncoder(w).Encode(book)
			return
		}
	}
	w.WriteHeader(http.StatusNotFound)
}

func deleteBook(w http.ResponseWriter, r *http.Request) {
	w.Header().Set("Content-Type", DefaultContentType)
	params := mux.Vars(r)
	for index, item := range books {
		if item.ID == params["id"] {
			books = append(books[:index], books[index+1:]...)
			break
		}
	}
	w.WriteHeader(http.StatusOK)
}

func wrappedRoute(f func(http.ResponseWriter, *http.Request, *pgx.Conn), c *pgx.Conn) func(http.ResponseWriter, *http.Request) {
	return func(w http.ResponseWriter, r *http.Request) {
		f(w, r, c)
	}
}

func main() {
	{
		connectionString := os.Getenv(EnvironmentDatabaseUrl)
		if len(connectionString) == 0 {
			connectionString = DefaultConnectionString
		}

		conn, err := pgx.Connect(context.Background(), connectionString)
		if err != nil {
			log.Fatalf("Unable to connection to database: %v\n", err)
			os.Exit(1)
		}
		defer conn.Close(context.Background())

		var name string
		var weight int64
		var nextNumber int64
		err = conn.QueryRow(context.Background(), "SELECT 'name' AS name, 2 AS weight, $1::int4 AS next_number", 42).Scan(&name, &weight, &nextNumber)
		if err != nil {
			log.Fatalf("QueryRow failed: %v\n", err)
			os.Exit(1)
		}
		log.Println(name, weight, nextNumber)
	}
	fs := http.FileServer(http.Dir("resources"))
	r := mux.NewRouter()
	books = append(books, Book{ID: "1", Title: "Война и Мир", Author: &Author{FirstName: "Лев", LastName: "Толстой"}})
	books = append(books, Book{ID: "2", Title: "Преступление и наказание", Author: &Author{FirstName: "Фёдор", LastName: "Достоевский"}})
	r.HandleFunc("/books", getBooks).Methods("GET")
	r.HandleFunc("/books", createBook).Methods("POST")
	r.HandleFunc("/books/{id}", getBook).Methods("GET")
	r.HandleFunc("/books/{id}", updateBook).Methods("PUT")
	r.HandleFunc("/books/{id}", deleteBook).Methods("DELETE")
	r.Handle("/static/*", http.StripPrefix("/static/", fs))
	loggedRouter := handlers.LoggingHandler(os.Stdout, r)
	log.Fatal(http.ListenAndServe(":8000", handlers.CORS()(loggedRouter)))
}
