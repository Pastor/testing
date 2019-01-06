#![feature(proc_macro_hygiene, decl_macro)]
#![allow(unused_imports)]

#[macro_use]
extern crate log;
//extern crate env_logger;
#[macro_use]
extern crate rocket;
#[macro_use]
extern crate rocket_contrib;
extern crate rusqlite;
#[macro_use]
extern crate serde_derive;

use std::borrow::Borrow;
use std::borrow::BorrowMut;
use std::collections::HashMap;
use std::sync::Mutex;

use rocket::outcome::Outcome::*;
use rocket::request::{self, FromRequest, Request, State};
use rocket_contrib::json::{Json, JsonValue};
use rocket::fairing::AdHoc;

use rusqlite::{Connection, Error};

#[cfg(test)]
mod tests;


#[derive(Serialize, Deserialize)]
struct Message {
    id: Option<ID>,
    contents: String,
}

struct Server {
    map: HashMap<ID, String>
}

type ID = usize;
type ServerState = Mutex<Server>;

struct AuthorizeGuard(ID);

impl Server {
    fn new() -> Server {
        Server { map: HashMap::<ID, String>::new() }
    }
}

impl<'a, 'r> FromRequest<'a, 'r> for AuthorizeGuard {
    type Error = ();

    fn from_request(req: &'a Request<'r>) -> request::Outcome<Self, ()> {
        let _state = req.guard::<State<ServerState>>()?;
        let _authorize = req.headers().get_one("Authorize");
        Success(AuthorizeGuard(0))
    }
}

#[post("/authorized", format = "json")]
fn authorized(_authorized: AuthorizeGuard, _map: State<ServerState>) -> JsonValue {
    json!({ "status": "success", "reason": "Authorized" })
}


// TODO: This example can be improved by using `route` with multiple HTTP verbs.
#[post("/<id>", format = "json", data = "<message>")]
fn new(id: ID, message: Json<Message>, state: State<ServerState>) -> JsonValue {
    let mut mutex_guard = state.lock().expect("map lock.");
    let hashmap = mutex_guard.map.borrow_mut();
    if hashmap.contains_key(&id) {
        warn!("Entity '{}' not found", id);
        json!({
            "status": "error",
            "reason": "ID exists. Try put."
        })
    } else {
        hashmap.insert(id, message.0.contents);
        json!({ "status": "ok" })
    }
}

#[put("/<id>", format = "json", data = "<message>")]
fn update(id: ID, message: Json<Message>, state: State<ServerState>) -> Option<JsonValue> {
    let mut mutex_guard = state.lock().expect("map lock.");
    let hashmap = mutex_guard.map.borrow_mut();
    if hashmap.contains_key(&id) {
        hashmap.insert(id, message.0.contents);
        Some(json!({ "status": "ok" }))
    } else {
        warn!("Entity '{}' not found", id);
        None
    }
}

#[get("/<id>", format = "json")]
fn get(id: ID, state: State<ServerState>) -> Option<Json<Message>> {
    let mutex_guard = state.lock().expect("map lock.");
    let hashmap = mutex_guard.map.borrow();
    hashmap.get(&id).map(|contents| {
        Json(Message {
            id: Some(id),
            contents: contents.clone(),
        })
    })
}

#[catch(404)]
fn not_found(req: &Request) -> JsonValue {
    json!({
        "status": "error",
        "reason": format!("Resource '{}' was not found.", req.uri())
    })
}

/// # Examples
///
/// ```rust
/// use rusqlite::{Connection, Error};
///
/// let conn = Connection::open_in_memory().expect("in memory db");
/// init_database(&conn);
/// ```
fn init_database(conn: &Connection) {
    conn.execute("CREATE TABLE entries (
                  id              INTEGER PRIMARY KEY,
                  name            TEXT NOT NULL
                  )", &[])
        .expect("create entries table");

    conn.execute("INSERT INTO entries (id, name) VALUES ($1, $2)",
                 &[&0, &"Rocketeer"])
        .expect("insert single entry into entries table");
}

fn main_rocket() -> rocket::Rocket {
    let conn = Connection::open_in_memory().expect("in memory db");
    init_database(&conn);
    rocket::ignite()
        .mount("/message", routes![new, update, get, authorized])
        .register(catchers![not_found])
        .attach(AdHoc::on_launch("hi", |rocket| {
            let hi = rocket.config().get_str("hi").unwrap_or("Default");
            info!("hi: {}", hi);
        }))
        .manage(Mutex::new(Server::new()))
}

fn main() {
//    env_logger::init();
    main_rocket().launch();
}
