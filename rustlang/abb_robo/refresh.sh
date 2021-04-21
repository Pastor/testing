#!/bin/sh

export DATABASE_URL=~/abb.db3

rm -f $DATABASE_URL
diesel migration run
diesel setup
diesel migration run
diesel setup

echo URL: $DATABASE_URL
