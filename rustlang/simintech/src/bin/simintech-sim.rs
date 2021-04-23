extern crate libc;

use libc::size_t;

#[link(name = "simintech")]
extern "C" {
    fn tick(c: size_t) -> size_t;
}

fn main() {
    let x = unsafe { tick(100) };
    println!("Count: {}", x);
}
