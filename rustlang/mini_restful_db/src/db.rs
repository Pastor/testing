extern crate rusqlite;
extern crate time;

extern crate serde;
extern crate serde_json;

use self::time::Timespec;
use self::rusqlite::{Connection, Result, Statement};
use self::rusqlite::types::ToSql;
use std::path::Path;
use std::fs;
use std::iter::Iterator;

#[derive(Debug, Clone)]
pub struct Article {
    pub id: i32,
    pub title: String,
    pub desc: String,
    pub content: String,
    pub created_at: Timespec
}

pub trait ToArticle {
    fn to_article(&self) -> Article;
}

pub struct Service {
    c: Connection
}

fn get_result(stmt: &mut Statement, params: &[&ToSql]) -> Vec<Article> {
    let articles = stmt.query_map(params, |row| {
        Article {
            id: row.get(0),
            title: row.get(1),
            desc: row.get(2),
            content: row.get(3),
            created_at: time::get_time()
        }
    }).unwrap();
    articles.map(|article| article.unwrap()).collect()
}

impl Service {
    #[allow(dead_code)]
    pub fn select(&self) -> Result<Vec<Article>> {
        let mut stmt = self.c.prepare("SELECT id, title, desc, content FROM articles")?;
        Ok(get_result(&mut stmt, &[]))
    }

    pub fn insert(&self, object: &ToArticle) -> Result<i32> {
        let mut stmt = self.c.prepare("INSERT INTO articles(title, desc, content) VALUES(?, ?, ?)")?;
        let article = object.to_article();
        let id = stmt.insert(&[&article.title, &article.desc, &article.content])? as i32;
        Ok(id)
    }

    pub fn update(&self, id: i32, object: &ToArticle) -> Result<()> {
        let mut stmt = self.c.prepare("UPDATE articles SET title = ?, desc = ?, content = ? WHERE id = ?")?;
        let article = object.to_article();
        stmt.execute(&[&article.title, &article.desc, &article.content, &id])?;
        Ok(())
    }

    pub fn select_by_id(&self, id: i32) -> Result<Option<Article>> {
        let mut stmt = self.c.prepare("SELECT id, title, desc, content FROM articles WHERE id = ?")?;
        let v = get_result(&mut stmt, &[&id]);
        let o = v.first();
        Ok(Some(o.unwrap().clone()))
    }
}

fn create(c: &Connection) -> Result<()> {
    c.execute(
        "CREATE TABLE articles(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, title TEXT NOT NULL, desc TEXT NOT NULL, content TEXT NOT NULL)", &[])?;
    Ok(())
}

fn insert(c: &Connection) -> Result<()> {
    let mut stmt = c.prepare("INSERT INTO articles(title, desc, content) VALUES(?, ?, ?)")?;
    for i in 0..100 {
        stmt.execute(&[
            &format!("Title_{:03}", i), &format!("Desc_{:03}", i), &format!("Content_{:03}", i)])?;
    }
    Ok(())
}


#[allow(unused_must_use)]
pub fn init() -> Service {
    let path = Path::new("sample.db");
    fs::remove_file(path);
    let c: Connection = Connection::open(path).unwrap();

    create(&c).unwrap();
    insert(&c).unwrap();
    Service { c: c }
}
