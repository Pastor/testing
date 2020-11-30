#[macro_use] extern crate log;
use std::io::{Error, Read, Write};
use std::net::{Shutdown, TcpStream};
use std::str::from_utf8;
use log::{debug, error, warn, info};
use reqwest::Result;
use reqwest::blocking::Client;

//55º 33' 43'' N 37º 34' 53'' E
//https://github.com/welshdave/simple-aprs.git

/*
function aprspass ($callsign) {
	$stophere = strpos($callsign, '-');
	if ($stophere) $callsign = substr($callsign, 0, $stophere);
	$realcall = strtoupper(substr($callsign, 0, 10));

	// initialize hash
	$hash = 0x73e2;
	$i = 0;
	$len = strlen($realcall);

	// hash callsign two bytes at a time
	while ($i < $len) {
		$hash ^= ord(substr($realcall, $i, 1))<<8;
		$hash ^= ord(substr($realcall, $i + 1, 1));
		$i += 2;
	}

	// mask off the high bit so number is always positive
	return $hash & 0x7fff;
}
 */

// const URL_RAW: &str = "russia.aprs2.net:14580";
const URL_RAW: &str = "euro.aprs2.net:14580";
const URL_HTTP: &str = "http://russia.aprs2.net:8080";
// const URL_HTTP: &str = "http://euro.aprs2.net:8080";
// const URL_HTTP: &str = "http://srvr.aprs-is.net:8080";


// const URL: &str = "euro.aprs2.net:14580";
// const URL: &str = "www.aprs.qrz.ru:14580";
// const URL: &str = "www.aprs.org:14580";
const USERNAME: &str = "R2BBH-7";
// const PASSWORD: &str = "PASSW";
const PASSWORD: &str = "11154";
const FILTER: &str = "";
// const FILTER: &str = "/";

fn handle_client(mut stream: &TcpStream) {
    let mut data = [0 as u8; 512];
    while match stream.read(&mut data) {
        Ok(size) => {
            stream.write(&data[0..size]).unwrap();
            true
        }
        Err(_) => {
            error!("An error occurred, terminating connection with {}", stream.peer_addr().unwrap());
            stream.shutdown(Shutdown::Both).unwrap();
            false
        }
    } {}
}

fn main() -> Result<()> {
    env_logger::init();
    raw_request()
    // http_request()
}

fn http_request() -> Result<()> {
    let client = reqwest::blocking::Client::new();
    let login = login_request_text();
    let res = client.post(URL_HTTP)
        .body(login + USERNAME + ">APRS,TCPIP*:>This is a test packet\r\n")
        .header("Accept-Type", "text/plain")
        .header("Content-Type", "application/octet-stream")
        .send()?;
    debug!("Status: {}", res.status());
    debug!("Headers:\n{:#?}", res.headers());
    let body = res.text()?;
    debug!("Body:\n{}", body);
    Ok(())
}

fn raw_request() -> Result<()> {
    match TcpStream::connect(URL_RAW) {
        Ok(mut stream) => {
            info!("Successfully connected to server.");
            let format = login_request_text();
            let msg = format.as_bytes();
            stream.write(msg).unwrap();
            stream.flush().unwrap();
            info!("Sent message.");
            handle_client(&stream)
        }
        Err(e) => {
            error!("Failed to connect: {}", e)
        }
    }
    warn!("Terminated.");
    Ok(())
}

fn login_request_text() -> String {
    return if FILTER.len() == 0 {
        format!("user {} pass {} vers QAPRS v1\r\n",
                USERNAME, PASSWORD)
    } else {
        format!("user {} pass {} vers QAPRS v1 filter {}\r\n",
                USERNAME, PASSWORD, FILTER)
    }
}
