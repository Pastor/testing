extern crate cc;

use std::path::Path;
use std::{env, fs};

fn main() {
    //    env::vars().into_iter().all(|(name, value)| {
    //        println!("{} = {}", name, value);
    //        true
    //    });
    let out_dir = env::var("OUT_DIR").unwrap();
    let dest_file = Path::new(&out_dir).join("Configuration.toml");
    fs::copy("Configuration.toml", dest_file).unwrap();

    cc::Build::new().file("src_c/sqlite3.c").compile("sqlite3");
    println!(r"cargo:rustc-link-search=D:\msys64\mingw64\lib");
}
