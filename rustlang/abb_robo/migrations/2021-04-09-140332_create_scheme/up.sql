CREATE TABLE users
(
    id            INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    first_name    TEXT NOT NULL,
    last_name     TEXT    DEFAULT NULL,
    username      TEXT    DEFAULT NULL,
    is_bot        BOOLEAN DEFAULT FALSE,
    language_code TEXT    DEFAULT NULL
);

CREATE TABLE chats
(
    id          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    title       TEXT NOT NULL,
    username    TEXT DEFAULT NULL,
    invite_link TEXT DEFAULT NULL
);

CREATE TABLE message_text
(
    id          INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    external_id INTEGER NOT NULL,
    created_at  INTEGER NOT NULL,
    creator     INTEGER DEFAULT NULL,
    chat        INTEGER DEFAULT NULL,
    data        TEXT    DEFAULT NULL
);
