#![feature(plugin)]
#![plugin(rocket_codegen)]

extern crate rocket;
extern crate serde_json;
extern crate serde;
extern crate time;
#[macro_use]
extern crate rocket_contrib;
#[macro_use]
extern crate serde_derive;

mod db;

use db::{Service, Article, ToArticle};

use rocket_contrib::{JSON, Value};
use rocket::{State};
use rocket::config::{Config, Environment};
use rocket::logger::LoggingLevel;
use std::sync::Mutex;

type ID = usize;
type ServiceContext = Mutex<Service>;

#[derive(Serialize, Deserialize, Debug)]
struct Payload {
    id: Option<ID>,
    title: String,
    desc: String,
    content: String
}

impl ToArticle for Payload {
    fn to_article(&self) -> Article {
        let id = if self.id.is_none() { 0i32 } else { self.id.unwrap() as i32 };
        Article {
            id: id,
            title: self.title.clone(),
            desc: self.desc.clone(),
            content: self.content.clone(),
            created_at: time::get_time()
        }
    }
}

#[post("/", format = "application/json", data = "<payload>")]
fn new(payload: JSON<Payload>, context: State<ServiceContext>) -> JSON<Value> {
    let ctx = context.lock().expect("map lock.");
    let id = ctx.insert(&payload.into_inner()).unwrap();
    JSON(json!({ "status": "ok", "id" : id}))
}

#[put("/<id>", format = "application/json", data = "<payload>")]
fn update(id: ID, payload: JSON<Payload>, context: State<ServiceContext>) -> Option<JSON<Value>> {
    let ctx = context.lock().unwrap();
    ctx.update(id as i32, &payload.into_inner()).unwrap();
    Some(JSON(json!({ "status": "ok" })))
}

#[get("/<id>", format = "application/json")]
fn get(id: ID, context: State<ServiceContext>) -> Option<JSON<Payload>> {
    println!("get({})", id);
    let ctx = context.lock().unwrap();
    let result = ctx.select_by_id(id as i32).unwrap();
    if result.is_some() {
        let article = result.unwrap();
        Some(JSON(Payload {
            id: Some(article.id as usize),
            title: article.title,
            desc: article.desc,
            content: article.content
        }))
    } else {
        None
    }
}

#[error(404)]
fn not_found() -> JSON<Value> {
    JSON(json!({
        "status": "error",
        "reason": "Resource was not found."
    }))
}

fn rocket() -> rocket::Rocket {
    let service = db::init();
    //    let config = Config::build(Environment::Staging)
    //        .address("0.0.0.0")
    //        .port(8000)
    //        .finalize().unwrap();
    //    rocket::Rocket::custom(config, true)
    rocket::logger::init(LoggingLevel::Debug);
    rocket::ignite()
        .mount("/message", routes![new, update, get])
        .catch(errors![not_found])
        .manage(Mutex::new(service))
}

fn main() {
    rocket().launch();
}

//TODO: https://rocket.rs/guide/testing/
#[cfg(test)]
mod test {
    use super::rocket;
    use rocket::testing::MockRequest;
    use rocket::http::{Status, Method};
    use rocket::http::{ContentType, Header};

    #[test]
    fn get() {
        let rocket = rocket::ignite().mount("/message", routes![super::get]);
        let mut req = MockRequest::new(Method::Get, "/message/10");
        req.add_header(Header::new("Accept-Type", "application/json"));
        let mut response = req.dispatch_with(&rocket);
        assert_eq!(response.status(), Status::Ok);
    }
}

