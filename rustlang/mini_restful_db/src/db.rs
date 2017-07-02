extern crate rusqlite;
extern crate time;

use self::time::Timespec;
use self::rusqlite::Connection;
use std::path::Path;
use std::fs;

#[derive(Debug)]
pub struct Article {
    id: i32,
    title: String,
    desc: String,
    content: String,
    created_at: Timespec,
    // TODO: data: Option<Vec<u8>>
}

pub fn init() {
    let path = Path::new("sample.db");
    fs::remove_file(path).unwrap();
    let conn = Connection::open(path).unwrap();

    conn.execute(
        "CREATE TABLE articles(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, title TEXT NOT NULL, desc TEXT NOT NULL, content TEXT NOT NULL)", &[]).unwrap();

    let me = Article {
        id: 0,
        title: format!("Dan"),
        desc: format!("Dan"),
        content: format!("Dan"),
        created_at: time::get_time(),
    };
    {
        conn.execute("INSERT INTO articles(title, desc, content) VALUES(?1, ?2, ?3)",
                     &[&me.title, &me.desc, &me.content]).unwrap();
    }
}
