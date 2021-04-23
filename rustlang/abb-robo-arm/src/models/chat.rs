#[allow(unused_imports)]
use serde::de::{Deserialize, Deserializer, Error};

use crate::schema::*;

#[derive(
    Queryable, Insertable, Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash, Deserialize,
)]
#[table_name = "chats"]
pub struct Chat {
    pub id: i32,
    pub title: String,
    pub username: Option<String>,
    pub invite_link: Option<String>,
}
