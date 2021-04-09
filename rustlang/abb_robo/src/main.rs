#[macro_use]
extern crate diesel;
#[macro_use]
extern crate serde_derive;
extern crate bloom;
extern crate serde_json;

use std::env;
use std::time::Duration;

use futures::StreamExt;
use telegram_bot::prelude::*;
use telegram_bot::{Api, Error, Integer, Message, MessageKind, ParseMode, UpdateKind};
use tokio::time::sleep;
use tracing::Level;

pub mod cache;
pub mod db;
pub mod models;
pub mod schema;

pub use crate::db::*;
pub use crate::models::*;

async fn test_message(api: Api, message: Message) -> Result<(), Error> {
    api.send(message.text_reply("Simple message")).await?;

    let mut reply = message.text_reply("`Markdown message`");
    api.send(reply.parse_mode(ParseMode::Markdown)).await?;

    let mut reply = message.text_reply("<b>Bold HTML message</b>");

    api.send(reply.parse_mode(ParseMode::Html)).await?;
    Ok(())
}

async fn test_preview(api: Api, message: Message) -> Result<(), Error> {
    api.send(message.text_reply("Message with preview https://telegram.org"))
        .await?;

    let mut reply = message.text_reply("Message without preview https://telegram.org");

    api.send(reply.disable_preview()).await?;
    Ok(())
}

async fn test_reply(api: Api, message: Message) -> Result<(), Error> {
    api.send(message.text_reply("Reply to message")).await?;
    api.send(message.chat.text("Text to message chat")).await?;

    api.send(message.from.text("Private text")).await?;
    Ok(())
}

async fn test_forward(api: Api, message: Message) -> Result<(), Error> {
    api.send(message.forward(&message.chat)).await?;

    api.send(message.forward(&message.from)).await?;
    Ok(())
}

async fn test_edit_message(api: Api, message: Message) -> Result<(), Error> {
    let message1 = api.send(message.text_reply("Round 1")).await?;

    sleep(Duration::from_secs(2)).await;

    let message2 = api.send(message1.edit_text("Round 2")).await?;

    sleep(Duration::from_secs(4)).await;

    api.send(message2.edit_text("Round 3")).await?;
    Ok(())
}

async fn test_get_chat(api: Api, message: Message) -> Result<(), Error> {
    let chat = api.send(message.chat.get_chat()).await?;
    api.send(chat.text(format!("Chat id {}", chat.id())))
        .await?;
    Ok(())
}

async fn test_get_chat_administrators(api: Api, message: Message) -> Result<(), Error> {
    let administrators = api.send(message.chat.get_administrators()).await?;
    let mut response = Vec::new();
    for member in administrators {
        response.push(member.user.first_name.clone())
    }
    api.send(message.text_reply(format!("Administrators: {}", response.join(", "))))
        .await?;
    Ok(())
}

async fn test_get_chat_members_count(api: Api, message: Message) -> Result<(), Error> {
    let count = api.send(message.chat.get_members_count()).await?;
    api.send(message.text_reply(format!("Members count: {}", count)))
        .await?;
    Ok(())
}

async fn test_get_chat_member(api: Api, message: Message) -> Result<(), Error> {
    let member = api.send(message.chat.get_member(&message.from)).await?;
    let first_name = member.user.first_name.clone();
    let status = member.status;
    api.send(message.text_reply(format!("Member {}, status {:?}", first_name, status)))
        .await?;
    Ok(())
}

async fn test_get_user_profile_photos(api: Api, message: Message) -> Result<(), Error> {
    let photos = api.send(message.from.get_user_profile_photos()).await?;

    api.send(message.text_reply(format!("Found photos: {}", photos.total_count)))
        .await?;
    Ok(())
}

async fn test_leave(api: Api, message: Message) -> Result<(), Error> {
    api.send(message.chat.leave()).await?;
    Ok(())
}

async fn test(api: Api, message: Message) -> Result<(), Error> {
    match message.kind {
        MessageKind::Text { ref data, .. } => match data.as_str() {
            "/message" => test_message(api, message).await?,
            "/preview" => test_preview(api, message).await?,
            "/reply" => test_reply(api, message).await?,
            "/forward" => test_forward(api, message).await?,
            "/edit-message" => test_edit_message(api, message).await?,
            "/get_chat" => test_get_chat(api, message).await?,
            "/get_chat_administrators" => test_get_chat_administrators(api, message).await?,
            "/get_chat_members_count" => test_get_chat_members_count(api, message).await?,
            "/get_chat_member" => test_get_chat_member(api, message).await?,
            "/get_user_profile_photos" => test_get_user_profile_photos(api, message).await?,
            "/leave" => test_leave(api, message).await?,
            _ => (),
        },
        _ => (),
    };

    Ok(())
}

//cargo install diesel_cli --no-default-features --features "sqlite"

#[tokio::main]
async fn main() -> Result<(), Error> {
    let mut db = Db::new();

    let file_appender = tracing_appender::rolling::hourly("logs", "features.log");
    let (non_blocking, _guard) = tracing_appender::non_blocking(file_appender);
    let subscriber = tracing_subscriber::fmt()
        .with_writer(non_blocking)
        .with_max_level(Level::INFO)
        .finish();
    tracing::subscriber::set_global_default(subscriber).unwrap();
    let token = env::var("TELEGRAM_BOT_TOKEN").expect("TELEGRAM_BOT_TOKEN not set");

    let api = Api::new(token);
    let mut stream = api.stream();

    while let Some(update) = stream.next().await {
        let update = update?;
        match update.kind {
            UpdateKind::Message(message) => {
                let id: Integer = message.from.id.into();
                test(api.clone(), message.clone()).await?;
                db.add_user(User {
                    id: id as i32,
                    first_name: message.from.first_name,
                    last_name: message.from.last_name,
                    username: message.from.username,
                    is_bot: message.from.is_bot,
                    language_code: message.from.language_code,
                });
            }
            UpdateKind::UpdateId(id) => tracing::info!("{}", id),
            UpdateKind::EditedMessage(message) => {
                tracing::info!("{:?}", message.clone());
            }
            UpdateKind::ChannelPost(post) => {
                tracing::info!("{:?}", post)
            }
            UpdateKind::EditedChannelPost(post) => {
                tracing::info!("{:?}", post)
            }
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
            UpdateKind::Error(error) => tracing::info!("{:?}", error),
            UpdateKind::Unknown => tracing::info!("Unknown"),
        }
    }
    Ok(())
}
