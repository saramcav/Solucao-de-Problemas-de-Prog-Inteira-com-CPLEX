#!/bin/bash

make

echo "Arquivos passados: $@"

for arquivo in "$@"; do
    ./coloracao "$arquivo" 1

    ./coloracao "$arquivo" 0
done