use serde::de::{Deserialize, Deserializer, Error};

use crate::models::*;
use crate::schema::*;

#[derive(
    Queryable, Insertable, Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash, Deserialize,
)]
#[table_name = "users"]
pub struct User {
    pub id: i32,
    pub first_name: String,
    pub last_name: Option<String>,
    pub username: Option<String>,
    pub is_bot: bool,
    pub language_code: Option<String>,
}
