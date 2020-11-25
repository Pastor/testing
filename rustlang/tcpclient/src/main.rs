use std::io::{Error, Read, Write};
use std::net::{Shutdown, TcpStream};
use std::str::from_utf8;

// const URL: &str = "russia.aprs2.net:14580";
// const URL: &str = "www.aprs.qrz.ru:14580";
const URL: &str = "www.aprs.org:14580";
const USERNAME: &str = "R2BBH";
const PASSWORD: &str = "PASSW";
const FILTER: &str = "/";

fn handle_client(mut stream: TcpStream) {
    let mut data = [0 as u8; 256];
    while match stream.read(&mut data) {
        Ok(size) => {
            stream.write(&data[0..size]).unwrap();
            true
        }
        Err(_) => {
            println!("An error occurred, terminating connection with {}", stream.peer_addr().unwrap());
            stream.shutdown(Shutdown::Both).unwrap();
            false
        }
    } {}
}

fn main() {
    match TcpStream::connect(URL) {
        Ok(mut stream) => {
            println!("Successfully connected to server.");

            let format = format!("user {} pass {} vers QAPRS v1 filter {} \r\n",
                                 USERNAME, PASSWORD, FILTER);
            let msg = format.as_bytes();
            stream.write(msg).unwrap();
            println!("Sent message.");
            handle_client(stream)
        }
        Err(e) => {
            println!("Failed to connect: {}", e)
        }
    }
    println!("Terminated.");
}
