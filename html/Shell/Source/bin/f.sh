#!/bin/bash
touch "$1/man"
echo "$1" > "$1/man"
vim -c 'startinsert' "$1/man"
