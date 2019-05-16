extern crate cc;

fn main() {
    cc::Build::new().file("src_c/sqlite3.c").compile("sqlite3");
    println!(r"cargo:rustc-link-search=D:\msys64\mingw64\lib");
}