#!/bin/bash

gcc -ggdb -Wall -Wextra utf8helper.c tests.c -o tests
gcc -ggdb -Wall -Wextra utf8helper.c utf8_valid.c -o utf8_valid
