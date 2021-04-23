extern crate abbrws;
extern crate bloom;
extern crate diesel;
extern crate hyper;
extern crate serde_derive;
extern crate serde_json;

use std::panic::PanicInfo;
use std::sync::{Arc, Mutex};
use std::{env, panic};

use abbrws::Client;
use futures::executor::block_on;
use futures::StreamExt;
use structopt::clap::AppSettings;
use structopt::StructOpt;
use telegram_bot::{Api, Error, UpdateKind};
use tracing::Level;

use abb_robo_arm::Database;
use abb_robo_arm::*;

//cargo install diesel_cli --no-default-features --features "sqlite"

#[tokio::main]
async fn main() -> Result<(), Error> {
    let mut db = Database::default();

    let file_appender = tracing_appender::rolling::hourly("logs", "features.jsonl");
    let (non_blocking, _guard) = tracing_appender::non_blocking(file_appender);
    let subscriber = tracing_subscriber::fmt()
        .with_writer(non_blocking)
        .with_max_level(Level::INFO)
        .json()
        .finish();
    tracing::subscriber::set_global_default(subscriber).unwrap();
    let token = env::var("TELEGRAM_BOT_TOKEN").expect("TELEGRAM_BOT_TOKEN not set");

    let api = Api::new(token);
    let mut stream = api.stream();

    let options = Options::from_args();

    let connect = || {
        abbrws::Client::new(&options.host, &options.user, &options.password)
            .map_err(|e| format!("failed to connect to {:?}: {}", options.host, e))
    };

    let client: Arc<Mutex<Client>> = Arc::new(Mutex::new(connect().unwrap()));
    event_abb_result("login", block_on(client.lock().unwrap().login()));
    event_abb_result(
        "mastership_request",
        block_on(client.lock().unwrap().mastership_request()),
    );

    let cc = Arc::clone(&client);
    panic::set_hook(Box::new(move |info: &PanicInfo| {
        tracing::error!("{:?}", info);
        event_abb_result(
            "mastership_release",
            block_on(cc.lock().unwrap().mastership_release()),
        );
    }));

    while let Some(update) = stream.next().await {
        let update = update?;
        match update.kind {
            UpdateKind::Message(message) => {
                test(api.clone(), message.clone()).await?;
                recv_message(api.clone(), &mut db, message)
            }
            UpdateKind::UpdateId(id) => tracing::info!("{}", id),
            UpdateKind::EditedMessage(message) => recv_message(api.clone(), &mut db, message),
            UpdateKind::ChannelPost(post) => recv_post(api.clone(), &mut db, post),
            UpdateKind::EditedChannelPost(post) => recv_post(api.clone(), &mut db, post),
            UpdateKind::InlineQuery(query) => tracing::info!("{:?}", query),
            UpdateKind::ChosenInlineResult(chosen) => {
                tracing::info!("{:?}", chosen)
            }
            UpdateKind::CallbackQuery(query) => tracing::info!("{:?}", query),
            UpdateKind::ShippingQuery(query) => tracing::info!("{:?}", query),
            UpdateKind::PreCheckoutQuery(query) => tracing::info!("{:?}", query),
            UpdateKind::Poll(poll) => tracing::info!("{:?}", poll),
            UpdateKind::PollAnswer(answer) => tracing::info!("{:?}", answer),
            UpdateKind::MyChatMember(member) => tracing::info!("{:?}", member),
            UpdateKind::ChatMember(member) => tracing::info!("{:?}", member),
            UpdateKind::Error(error) => tracing::error!("{:?}", error),
            UpdateKind::Unknown => tracing::error!("Unknown"),
        }
    }
    event_abb_result(
        "mastership_release",
        block_on(client.lock().unwrap().mastership_release()),
    );
    Ok(())
}

///
/// # Example
///
///```
/// event_abb_result("none", Ok(()));
/// ```
///
fn event_abb_result(name: &'static str, result: Result<(), abbrws::Error>) {
    match result {
        Ok(_) => tracing::debug!("ABB[{}]: success", name),
        Err(err) => tracing::error!("ABB[{}]: {:?}", name, err),
    }
}

#[derive(StructOpt)]
#[structopt(setting(AppSettings::DeriveDisplayOrder))]
#[structopt(setting(AppSettings::ColoredHelp))]
#[structopt(setting(AppSettings::UnifiedHelpMessage))]
struct Options {
    /// The host to connect to.
    #[structopt(long, short)]
    #[structopt(default_value = "127.0.0.1")]
    host: String,

    /// The user to authenticate as.
    #[structopt(long, short)]
    #[structopt(global = true)]
    #[structopt(default_value = "Default User")]
    user: String,

    /// The password for the user.
    #[structopt(long, short)]
    #[structopt(global = true)]
    #[structopt(default_value = "robotics")]
    password: String,
}
