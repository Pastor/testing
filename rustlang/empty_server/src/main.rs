#![feature(proc_macro_hygiene, decl_macro)]
#![allow(unused_imports)]

extern crate env_logger;
#[macro_use]
extern crate log;
#[macro_use]
extern crate rocket;
#[macro_use]
extern crate rocket_contrib;

use std::borrow::BorrowMut;
use std::borrow::{Borrow, Cow};
use std::cell::RefCell;
use std::collections::HashMap;
use std::ffi::OsString;
use std::io::{BufReader, Bytes, Cursor, Error, ErrorKind, Read, Result};
use std::sync::Mutex;
use std::{env, io, result};

use reqwest::blocking::Response;
use rocket::data::DataStream;
use rocket::fairing::AdHoc;
use rocket::outcome::Outcome::*;
use rocket::request::{self, FromRequest, Request};
use rocket::response::Stream;

mod store;
use store::{StoreS3, Store};

#[cfg(test)]
mod tests;

#[cfg(debug_assertions)]
fn proxy_url() -> Option<String> {
    match env::var_os("DOCSTORE_HOSTNAME") {
        Some(token) => Some(token.into_string().unwrap()),
        None => Some("http://ya.ru".to_string()),
    }
}

#[cfg(not(debug_assertions))]
fn proxy_url() -> Option<String> {
    match env::var_os("DOCSTORE_HOSTNAME") {
        Some(token) => Some(token.into_string().unwrap()),
        None => None,
    }
}

fn error_response(s: &str) -> rocket::Response {
    rocket::Response::build()
        .raw_header("Content-Type", "text/plain")
        .status(rocket::http::Status::BadRequest)
        .sized_body(Cursor::new(s))
        .finalize()
}

#[allow(non_snake_case, unused_variables, unused_mut)]
#[get("/?<GUID>&<SIG>")]
fn proxy<'r>(GUID: Option<String>, SIG: Option<String>) -> rocket::Response<'r> {
    let url = proxy_url();
    if url.is_some() && GUID.is_some() {
        let url = format!("{}/*/{}", url.unwrap(), GUID.unwrap());
        info!("Proxy request: '{}'", url);
        let mut resp = reqwest::blocking::get(url.as_str()).unwrap();
        return if resp.status().is_success() {
            #[cfg(feature = "enable_uncompressed")]
                {
                    let mut buf: Vec<u8> = vec![];
                    resp.copy_to(&mut buf).unwrap();
                    let reader = std::io::Cursor::new(buf);
                    let mut arch = zip::ZipArchive::new(reader).unwrap();
                    for i in 0..arch.len() {
                        let file = arch.by_index(i).unwrap();
                        if !file.name().contains("sig") {
                            return error_response("Find signature");
                        }
                    }
                }
            let mut response = rocket::Response::new();
            response.set_streamed_body(resp);
            return response;
        } else {
            error_response("Can't fetch GUID")
        };
    }
    error_response("Hostname for docstore not defined")
}

#[catch(404)]
fn not_found(req: &Request) -> String {
    format!("Resource '{}' was not found.", req.uri())
}

fn main_rocket() -> rocket::Rocket {
    rocket::ignite()
        .mount("/request", routes![proxy])
        .register(catchers![not_found])
}

fn main() {
    let _s = StoreS3::new("b".to_string());
    main_rocket().launch();
}
