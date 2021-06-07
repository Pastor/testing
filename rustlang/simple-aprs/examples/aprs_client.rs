extern crate pretty_env_logger;

use std::env;

use simple_aprs::*;

fn aprs_packet_handler(packet: APRSPacket) {
    match packet.parsed() {
        Ok(parsed) => {
            println!("Source: {:#?}", parsed)
        }
        Err(err) => {
            println!("Error parsing packet: {:?}", err);
            match String::from_utf8(packet.raw) {
                Ok(msg) => println!("{:?}", msg),
                Err(err) => println!("Error converting APRS packet to UTF8: {}", err),
            }
        }
    }
}

///http://www.aprs-is.net/javAPRSFilter.aspx
///
fn main() {
    pretty_env_logger::init();

    let args = arguments::parse(env::args()).unwrap();

    let callsign = args.get::<String>("callsign").unwrap();
    let passcode = args.get::<String>("passcode").unwrap();

    let settings = ISSettings::new(
        "euro.aprs2.net".to_string(),
        14580,
        callsign.to_string(),
        passcode.to_string(),
        "r/55/37/100".to_string(),
    );

    let aprs_is = IS::new(settings, aprs_packet_handler);

    match aprs_is.connect() {
        Ok(()) => println!("Disconnected"),
        Err(err) => println!("An error occured: {}", err),
    }
}
