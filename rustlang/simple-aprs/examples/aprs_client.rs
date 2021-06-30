#[macro_use]
extern crate log;
extern crate pretty_env_logger;
extern crate snailquote;

use std::env;

use aprs_parser::{APRSData, Timestamp};

use simple_aprs::*;

fn aprs_packet_handler(packet: APRSPacket) {
    match packet.parsed() {
        Ok(parsed) => {
            let data = match parsed.data {
                APRSData::Position(pos) => {
                    let comment = snailquote::unescape(pos.comment.as_str()).unwrap();
                    let timestamp = match pos.timestamp {
                        Some(timestamp) => match timestamp {
                            Timestamp::DDHHMM(d, h, m) => format!("{:02}:{:02}:{:02}d", d, h, m),
                            Timestamp::HHMMSS(h, m, s) => format!("{:02}:{:02}:{:02}h", h, m, s),
                            Timestamp::Unsupported(text) => text,
                        },
                        None => String::from("00:00:00u")
                    };
                    format!("{:010};{:010};{:9};{}",
                            pos.latitude, pos.longitude, timestamp, comment)
                }
                _ => String::new()
            };
            info!("From: {:6}, To: {:6}, Data: {:?}", parsed.from.call, parsed.to.call, data)
        }
            Err(err) => {
            error!("Error parsing packet: {:?}. Data: {:?}", err, packet.raw.clone());
            match String::from_utf8(packet.raw) {
                Ok(msg) => warn!("{:?}", msg),
                Err(err) => error!("Error converting APRS packet to UTF8: {}", err),
            }
        }
    }
}

///http://www.aprs-is.net/javAPRSFilter.aspx
///
fn main() {
    let env = pretty_env_logger::env_logger::Env::default()
        .filter_or(pretty_env_logger::env_logger::DEFAULT_FILTER_ENV, "info");
    pretty_env_logger::env_logger::Builder::from_env(env)
        .init();

    let args = arguments::parse(env::args()).unwrap();

    let callsign = args.get::<String>("callsign").unwrap();
    let passcode = args.get::<String>("passcode").unwrap();

    let settings = Settings::new(
        "euro.aprs2.net".to_string(),
        14580,
        callsign.to_string(),
        passcode.to_string(),
        "r/55/37/100".to_string(),
    );

    let aprs_is = Client::new(settings, aprs_packet_handler);

    match aprs_is.connect() {
        Ok(()) => info!("Disconnected"),
        Err(err) => error!("An error occured: {}", err),
    }
}
