CREATE TABLE users
(
    id            INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,
    first_name    TEXT NOT NULL,
    last_name     TEXT    DEFAULT NULL,
    username      TEXT    DEFAULT NULL,
    is_bot        BOOLEAN DEFAULT FALSE,
    language_code TEXT    DEFAULT NULL
);