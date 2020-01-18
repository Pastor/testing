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
use reqwest::blocking::Response;
use std::borrow::{Borrow, Cow};
use std::borrow::BorrowMut;
use std::cell::RefCell;
use std::collections::HashMap;
use std::io::{Bytes, Error, ErrorKind, Result, Read, BufReader};
use std::sync::Mutex;

use rocket::fairing::AdHoc;
use rocket::outcome::Outcome::*;
use rocket::request::{self, FromRequest, Request};
use rocket::response::Stream;
use rocket::data::DataStream;
use std::ffi::OsString;

#[cfg(test)]
mod tests;

struct ProxyRead {
    inner: Box<Response>
}

impl ProxyRead {
    fn new(r: Response) -> ProxyRead {
        ProxyRead { inner: Box::new(r) }
    }
}

impl Read for ProxyRead {
    fn read(&mut self, buf: &mut [u8]) -> io::Result<usize> {
        self.inner.read(buf)
    }
}

#[cfg(debug_assertions)]
fn proxy_url() -> Option<String> {
    match env::var_os("DOCSTORE_HOSTNAME") {
        Some(token) => Some(token.into_string().unwrap()),
        None => Some("http://ya.ru".to_string())
    }
}

#[cfg(not(debug_assertions))]
fn proxy_url() -> Option<String> {
    match env::var_os("DOCSTORE_HOSTNAME") {
        Some(ref token) => Some(token.into_string().unwrap()),
        None => None
    }
}

//<a href="https://github.com/rust-lang/rust/issues/41966">Ошибка</a>
//struct Dummy<'a> {
//    m: &'a [u8],
//    inner: Box<dyn Read>,
//}
//
//impl<'a> Dummy<'a> {
//    fn new<E: Read+Sized>(r: E) -> Dummy<'a> where E: 'a {
//        Dummy {inner: Box::new(r), m: vec![].as_slice()}
//    }
//}

#[allow(non_snake_case, unused_variables)]
#[get("/?<GUID>&<SIG>")]
fn proxy(GUID: Option<String>, SIG: Option<String>) -> io::Result<Stream<ProxyRead>> {
    let url = proxy_url();
    if url.is_some() && GUID.is_some() {
        let url = format!("{}/*/{}", url.unwrap(), GUID.unwrap());
        let resp = reqwest::blocking::get(url.as_str()).unwrap();
        return if resp.status().is_success() {
            #[cfg(feature = "enable_uncompressed")]
                {
                    let mut buf: Vec<u8> = vec![];
                    resp.copy_to(&mut buf).unwrap();
                    let mut arch = zip::ZipArchive::new(buf.as_slice()).unwrap();
                    for i in 0..arch.len() {
                        let mut file = arch.by_index(i).unwrap();
                        if !file.name().contains("sig") {
                            return Err(Error::new(ErrorKind::InvalidData, "Find signature"));
                        }
                    }
                }
            return Ok(Stream::from(ProxyRead::new(resp)));
        } else {
            Err(Error::new(ErrorKind::InvalidData, "Can't fetch GUID"))
        };
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
