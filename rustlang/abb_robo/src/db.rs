use diesel::prelude::*;
use dotenv::dotenv;
use std::env;

use crate::cache::Cache;
pub use crate::models::*;
use crate::schema::users::columns::id;
pub use crate::schema::*;
use std::cell::RefCell;

pub fn establish_connection() -> SqliteConnection {
    dotenv().ok();
    let database_url = match env::var("DATABASE_URL") {
        Ok(val) => val,
        Err(_) => String::from("~/telegram.db3"),
    };
    SqliteConnection::establish(&database_url)
        .unwrap_or_else(|e| panic!("Error connecting to {}: {:?}", database_url, e))
}

pub struct Db {
    connection: SqliteConnection,
    user_cache: RefCell<Cache>,
}

impl Db {
    pub fn new() -> Self {
        Db {
            connection: establish_connection(),
            user_cache: RefCell::new(Cache::default()),
        }
    }

    pub fn add_user(&mut self, user: User) {
        if !self.user_cache.get_mut().contains(user.id) {
            let ret = users::dsl::users
                .filter(id.eq(user.id))
                .select(id)
                .first::<i32>(&self.connection);
            if let Err(_) = ret {
                diesel::insert_into(users::table)
                    .values(&user)
                    .execute(&self.connection)
                    .expect("Error saving new user");
            }
            self.user_cache.get_mut().store(user.id)
        }
    }
}
