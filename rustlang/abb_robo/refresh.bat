@echo off

rem cargo install diesel_cli --no-default-features --features sqlite

set DATABASE_URL=C:/Users/pastor/abb.db3

diesel migration run
diesel setup
diesel migration run
diesel setup
