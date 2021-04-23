use std::cell::RefCell;
use std::env;
use std::path::PathBuf;

use diesel::prelude::*;
use dotenv::dotenv;

use crate::cache::Cache;
pub use crate::models::*;
pub use crate::schema::*;

fn establish_connection() -> SqliteConnection {
    dotenv().ok();
    let database_url = match env::var("DATABASE_URL") {
        Ok(val) => val,
        Err(_) => {
            let home = match env::var("HOME") {
                Ok(home) => home,
                Err(_) => env::current_dir()
                    .unwrap_or_else(|_| PathBuf::from("~"))
                    .to_str()
                    .unwrap()
                    .to_string(),
            };
            let home = PathBuf::from(home);
            let db = home.join("abb.db3");
            db.to_str().unwrap().to_string()
        }
    };
    SqliteConnection::establish(&database_url)
        .unwrap_or_else(|e| panic!("Error connecting to {}: {:?}", database_url, e))
}

pub struct Database {
    connection: SqliteConnection,
    user_cache: RefCell<Cache>,
    chat_cache: RefCell<Cache>,
}

impl Default for Database {
    fn default() -> Self {
        Database {
            connection: establish_connection(),
            user_cache: RefCell::new(Cache::default()),
            chat_cache: RefCell::new(Cache::default()),
        }
    }
}

impl Database {
    pub fn add_message_text(&mut self, message: MsgText) {
        diesel::insert_into(message_text::table)
            .values(&message)
            .execute(&self.connection)
            .expect("Error saving new user");
    }

    pub fn add_chat(&mut self, chat: Chat) {
        if !self.chat_cache.get_mut().contains(chat.id) {
            let ret = chats::dsl::chats
                .filter(chats::columns::id.eq(chat.id))
                .select(chats::columns::id)
                .first::<i32>(&self.connection);
            if ret.is_err() {
                diesel::insert_into(chats::table)
                    .values(&chat)
                    .execute(&self.connection)
                    .expect("Error saving new chat");
            }
            self.chat_cache.get_mut().store(chat.id)
        }
    }

    pub fn add_user(&mut self, user: User) {
        if !self.user_cache.get_mut().contains(user.id) {
            let ret = users::dsl::users
                .filter(users::columns::id.eq(user.id))
                .select(users::columns::id)
                .first::<i32>(&self.connection);
            if ret.is_err() {
                diesel::insert_into(users::table)
                    .values(&user)
                    .execute(&self.connection)
                    .expect("Error saving new user");
            }
            self.user_cache.get_mut().store(user.id)
        }
    }
}
