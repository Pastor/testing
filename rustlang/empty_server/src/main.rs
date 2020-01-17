#![feature(proc_macro_hygiene, decl_macro)]
#![allow(unused_imports)]

extern crate env_logger;
#[macro_use]
extern crate log;
#[macro_use]
extern crate rocket;
#[macro_use]
extern crate rocket_contrib;

use std::{env, io, result};
use reqwest::blocking::{Response};
use std::borrow::Borrow;
use std::borrow::BorrowMut;
use std::cell::RefCell;
use std::collections::HashMap;
use std::io::{Bytes, Error, ErrorKind, Result};
use std::sync::Mutex;

use rocket::fairing::AdHoc;
use rocket::outcome::Outcome::*;
use rocket::request::{self, FromRequest, Request};
use rocket::response::Stream;
use rocket::data::DataStream;
use std::ffi::OsString;

#[cfg(test)]
mod tests;

#[allow(non_snake_case)]
#[get("/?<GUID>&<SIG>")]
fn proxy(GUID: Option<String>, SIG: Option<String>) -> io::Result<Stream<DataStream>> {
    let hostname = env::var_os("DOCSTORE_HOSTNAME").or(Some(OsString::from("ya.ru")));
    if hostname.is_some() && GUID.is_some() {
        let url = format!("{}/*/{}", hostname.unwrap().into_string().unwrap(), GUID.unwrap());
        let rr = reqwest::blocking::get(url.as_str());

//        Ok(Stream::from(rr.bytes_stream()))
//    } else {
//        Err(Error::new(ErrorKind::InvalidInput, "Hostname for docstore not defined"))
    }
    Err(Error::new(ErrorKind::InvalidInput, "Hostname for docstore not defined"))
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
    main_rocket().launch();
}
