#[allow(unused_imports)]
use serde::de::{Deserialize, Deserializer, Error};

use crate::schema::*;

#[derive(
    Queryable, Insertable, Debug, Clone, PartialEq, PartialOrd, Eq, Ord, Hash, Deserialize,
)]
#[table_name = "message_text"]
pub struct MsgText {
    pub id: Option<i32>,
    pub creator: Option<i32>,
    pub external_id: i32,
    pub created_at: i32,
    pub chat: Option<i32>,
    pub data: Option<String>,
}
