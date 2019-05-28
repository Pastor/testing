use std::error::Error;
use std::fs::File;
use std::{fs, io, result};

use serde_derive::{Deserialize, Serialize};

#[derive(Deserialize, Serialize)]
pub struct Config {
    ip: Option<String>,
    port: Option<u16>,
    #[serde(rename = "instance")]
    instance: Vec<Instance>,
}

#[derive(Deserialize, Serialize)]
pub struct Instance {
    key: String,
    password: String,
    provider: String,
}

impl Config {
    #[inline(always)]
    pub fn new(filename: &'static str) -> Option<Config> {
        let text: String = fs::read_to_string(filename).unwrap();
        let config = toml::from_str(text.as_str()).unwrap();
        Some(config)
    }
}
