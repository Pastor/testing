#[allow(unused_imports)]
use serde::de::{Deserialize, Deserializer, Error};

use crate::schema::*;
use std::time::SystemTime;

#[derive(
    Queryable, Insertable, Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash, Deserialize,
)]
#[table_name = "message_text"]
pub struct Text {
    pub id: i32,
    pub created_at: SystemTime,
    pub chat: Option<i32>,
    pub data: Option<String>,
}
