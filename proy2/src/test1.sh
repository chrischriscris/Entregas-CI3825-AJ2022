#! /usr/bin/bash

if [ $# -ne 1 ]
then
    echo "Uso: $0 <dirname>"
    exit -1
fi

newl=0

for file in `find $1 -name "*.txt"`
do
    if [ $newl -eq 0 ]
    then
        cat $file >> expected_out.txt
        newl=1
    else
        echo >> expected_out.txt
        cat $file >> expected_out.txt
    fi
done

sort -g -o expected_out.txt expected_out.txt

./ordena $1 out.txt
diff expected_out.txt out.txt -w -q
rm expected_out.txt