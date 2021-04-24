extern crate libc;
extern crate rand;

use libc::size_t;
use rand::RngCore;

#[link(name = "simintech")]
extern "C" {
    fn tick(c: size_t) -> size_t;
}

fn main() {
    let x = unsafe {
        let mut rng = rand::thread_rng();
        tick(rng.next_u32() as size_t) };
    println!("Count: {}", x);
}
