#!/bin/bash

for i in $(seq 0 127); 
  do x=$(printf '%x ' $i); 
  echo -ne "\x$x is $x $i \n"; 
done
