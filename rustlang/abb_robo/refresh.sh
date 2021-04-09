#!/bin/sh

export DATABASE_URL=~/telegram.db3

rm -f $DATABASE_URL
diesel migration run
diesel setup
diesel migration run
diesel setup

echo URL: $DATABASE_URL
