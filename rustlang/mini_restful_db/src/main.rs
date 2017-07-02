#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;
extern crate serde_json;
extern crate serde;
#[macro_use] extern crate rocket_contrib;
#[macro_use] extern crate serde_derive;

#[cfg(test)] mod tests;
mod db;

use rocket_contrib::{JSON, Value};
use rocket::State;
use std::collections::HashMap;
use std::sync::Mutex;

type ID = usize;
type MessageMap = Mutex<HashMap<ID, String>>;

#[derive(Serialize, Deserialize, Debug)]
struct Message {
    id: Option<ID>,
    contents: String
}

// TODO: This example can be improved by using `route` with multiple HTTP verbs.
#[post("/<id>", format = "application/json", data = "<message>")]
fn new(id: ID, message: JSON<Message>, context: State<MessageMap>) -> JSON<Value> {
    let mut map = context.lock().expect("map lock.");
    if map.contains_key(&id) {
        JSON(json!({
            "status": "error",
            "reason": "ID exists. Try put."
        }))
    } else {
        map.insert(id, message.0.contents);
        JSON(json!({ "status": "ok" }))
    }
}

#[put("/<id>", format = "application/json", data = "<message>")]
fn update(id: ID, message: JSON<Message>, context: State<MessageMap>) -> Option<JSON<Value>> {
    let mut map = context.lock().unwrap();
    if map.contains_key(&id) {
        map.insert(id, message.0.contents);
        Some(JSON(json!({ "status": "ok" })))
    } else {
        None
    }
}

#[get("/<id>", format = "application/json")]
fn get(id: ID, context: State<MessageMap>) -> Option<JSON<Message>> {
    let map = context.lock().unwrap();
    map.get(&id).map(|contents| {
        JSON(Message {
            id: Some(id),
            contents: contents.clone()
        })
    })
}

#[error(404)]
fn not_found() -> JSON<Value> {
    JSON(json!({
        "status": "error",
        "reason": "Resource was not found."
    }))
}

fn rocket() -> rocket::Rocket {
    db::init();
    rocket::ignite()
        .mount("/message", routes![new, update, get])
        .catch(errors![not_found])
        .manage(Mutex::new(HashMap::<ID, String>::new()))
}

fn main() {
    rocket().launch();
}

