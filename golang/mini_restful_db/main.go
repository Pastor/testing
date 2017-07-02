package main

import "fmt"
import (
	"database/sql"
	"encoding/json"
	"flag"
	"log"
	"net/http"
	"time"

	"os"

	"github.com/gorilla/mux"
	_ "github.com/mattn/go-sqlite3"
)

const SIZE int = 100

type Article struct {
	Id      int    `json:"id"`
	Title   string `json:"title"`
	Desc    string `json:"desc"`
	Content string `json:"content"`
}

type Articles []Article

func read_article(rows *sql.Rows) Article {
	var id int
	var title string
	var desc string
	var content string
	article := Article{}
	sql_error := rows.Scan(&id, &title, &desc, &content)
	if sql_error != nil {
		fmt.Printf("Error %s", sql_error)
	} else {
		article = Article{id, title, desc, content}
	}
	return article
}

func all_articles(w http.ResponseWriter, r *http.Request, db *sql.DB) {
	articles := make([]Article, 0)
	fmt.Println("Endpoint Hit: all_articles")
	rows, sql_error := db.Query("SELECT id, title, desc, content FROM articles")
	if sql_error != nil {
		json.NewEncoder(w).Encode(sql_error)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	defer rows.Close()
	for rows.Next() {
		articles = append(articles, read_article(rows))
	}
	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(articles)
}

func single_article(w http.ResponseWriter, r *http.Request, db *sql.DB) {
	vars := mux.Vars(r)
	select_id := vars["id"]
	fmt.Printf("Endpoint Hit: article(%s)\n", select_id)
	stmt, sql_error := db.Prepare("SELECT id, title, desc, content FROM articles WHERE id = ?")
	if sql_error != nil {
		json.NewEncoder(w).Encode(sql_error)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	defer stmt.Close()
	rows, sql_error := stmt.Query(select_id)
	if sql_error != nil {
		json.NewEncoder(w).Encode(sql_error)
		w.WriteHeader(http.StatusInternalServerError)
		return
	}
	defer rows.Close()
	article := Article{}
	if rows.Next() {
		article = read_article(rows)
	}
	w.WriteHeader(http.StatusOK)
	json.NewEncoder(w).Encode(article)
}

func home_page(w http.ResponseWriter, r *http.Request, db *sql.DB) {
	fmt.Fprint(w, "Welcome")
	fmt.Println("Endpoint")
	w.WriteHeader(http.StatusOK)
}

func wrapped_route(f func(http.ResponseWriter, *http.Request, *sql.DB), db *sql.DB) func(http.ResponseWriter, *http.Request) {
	return func(w http.ResponseWriter, r *http.Request) {
		f(w, r, db)
	}
}

func main() {
	os.Remove("simple.db")
	db, sql_error := sql.Open("sqlite3", "simple.db")
	if sql_error != nil {
		log.Fatal(sql_error)
	}
	defer db.Close()
	sql_stmt := `
	CREATE TABLE articles(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, title TEXT NOT NULL, desc TEXT NOT NULL, content TEXT NOT NULL);
	DELETE FROM articles;
	`
	_, sql_error = db.Exec(sql_stmt)
	if sql_error != nil {
		log.Printf("%q: %s\n", sql_error, sql_stmt)
		return
	}
	tx, sql_error := db.Begin()
	if sql_error != nil {
		log.Fatal(sql_error)
	}
	stmt, sql_error := tx.Prepare("INSERT INTO articles(title, desc, content) VALUES(?, ?, ?)")
	if sql_error != nil {
		log.Fatal(sql_error)
	}
	defer stmt.Close()
	for i := 0; i < SIZE; i++ {
		_, sql_error = stmt.Exec(
			fmt.Sprintf("Title_%03d", i),
			fmt.Sprintf("Desc_%03d", i),
			fmt.Sprintf("Content_%03d", i))
		if sql_error != nil {
			log.Fatal(sql_error)
		}
	}
	tx.Commit()

	var dir string

	flag.StringVar(&dir, "dir", ".", "the directory to serve files from. Defaults to the current dir")
	flag.Parse()

	router := mux.NewRouter().StrictSlash(true)
	router.HandleFunc("/", wrapped_route(home_page, db))
	router.HandleFunc("/all", wrapped_route(all_articles, db)).Methods("GET")
	router.HandleFunc("/article/{id:[0-9]+}", wrapped_route(single_article, db))
	router.PathPrefix("/static/").Handler(http.StripPrefix("/static/", http.FileServer(http.Dir(dir))))

	server := &http.Server{
		Handler: router,
		Addr:    "127.0.0.1:8081",
		// Good practice: enforce timeouts for servers you create!
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}
	log.Fatal(server.ListenAndServe())
}
