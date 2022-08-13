#! /usr/bin/bash

echo gigante
time ./ordena testdir_gigante/ out.txt
echo grande
time ./ordena testdir_grande/ out.txt
echo mediano
time ./ordena  testdir_mediano/ out.txt