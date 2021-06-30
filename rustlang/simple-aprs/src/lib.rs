extern crate aprs_parser;

use std::error::Error;
use std::time::Duration;

use aprs_parser::{APRSError, APRSMessage};
use futures::sink::SinkExt;
use futures::StreamExt;
use log::{info, trace, warn};
use tokio::net::TcpStream;
use tokio::time;
use tokio_util::codec::{FramedRead, FramedWrite};

mod codec;

pub struct APRSPacket {
    pub raw: Vec<u8>,
}

impl APRSPacket {
    pub fn parsed(&self) -> Result<APRSMessage, APRSError> {
        match String::from_utf8(self.raw.clone()) {
            Ok(result) => aprs_parser::parse(result.as_str()),
            Err(_) => {
                let buf = self.raw.clone();
                let result = buf.iter()
                    .fold(String::new(), |mut acc, x| {
                        acc.push(char::from(*x));
                        acc
                    });
                aprs_parser::parse(result.as_str())
            }
        }
    }
}

pub struct Settings {
    pub host: String,
    pub port: u16,
    pub callsign: String,
    pub passcode: String,
    pub filter: String,
}

impl Settings {
    pub fn new(
        host: String,
        port: u16,
        callsign: String,
        passcode: String,
        filter: String,
    ) -> Settings {
        Settings {
            host,
            port,
            callsign,
            passcode,
            filter,
        }
    }
}

pub type PacketHandler = fn(APRSPacket);

pub struct Client {
    settings: Settings,
    packet_handler: PacketHandler,
}

impl Client {
    pub fn new(settings: Settings, packet_handler: PacketHandler) -> Client {
        Client {
            settings,
            packet_handler,
        }
    }

    #[tokio::main]
    pub async fn connect(&self) -> Result<(), Box<dyn Error>> {
        let address = format!("{}:{}", self.settings.host, self.settings.port);

        let stream = TcpStream::connect(address).await?;

        let (r, w) = stream.into_split();

        let mut writer = FramedWrite::new(w, codec::ByteLinesCodec::new());
        let mut reader = FramedRead::new(r, codec::ByteLinesCodec::new());

        let login_message = {
            let name = option_env!("CARGO_PKG_NAME").unwrap_or("unknown");
            let version = option_env!("CARGO_PKG_VERSION").unwrap_or("0.0.0");

            format!(
                "user {} pass {} vers {} {}{}",
                self.settings.callsign,
                self.settings.passcode,
                name,
                version,
                if self.settings.filter.is_empty() {
                    "".to_string()
                } else {
                    format!(" filter {}", self.settings.filter)
                }
            )
        };

        info!("Logging on to APRS server");
        trace!("Login message: {}", login_message);
        writer.send(login_message.as_bytes()).await?;

        tokio::spawn(async move {
            let mut interval = time::interval(Duration::from_secs(3600));
            loop {
                interval.tick().await;
                info!("Sending keep alive message to APRS server");
                writer.send("# keep alive".as_bytes()).await.unwrap();
            }
        });

        while let Some(packet) = reader.next().await {
            match packet {
                Ok(packet) => {
                    if packet[0] == b'#' {
                        match String::from_utf8(packet.to_vec()) {
                            Ok(server_message) => {
                                trace!("Received server response: {}", server_message);
                                if server_message.contains("unverified") {
                                    info!("User not verified on APRS server");
                                    continue;
                                }
                                if server_message.contains(" verified") {
                                    info!("User verified on APRS server");
                                }
                            }
                            Err(err) => warn!("Error processing server response: {}", err),
                        }
                    } else {
                        trace!("{:?}", packet);
                        (self.packet_handler)(APRSPacket {
                            raw: packet.to_vec(),
                        });
                    }
                }
                Err(err) => {
                    warn!("Error processing packet from APRS server: {}", err);
                }
            }
        }
        Ok(())
    }
}
