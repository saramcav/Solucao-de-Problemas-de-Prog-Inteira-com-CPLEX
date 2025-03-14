#!/bin/bash

make

echo "Arquivos passados: $@"

for arquivo in "$@"; do
    ./trab1 "$arquivo"
done