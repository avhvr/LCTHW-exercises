#!/bin/bash

set -xe

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat c 100 30

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat s 1 harsha harsha@allu.com

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat s 2 vasundhara vasundhara@allu.com

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat s 3 subbamma subbamma@allu.com

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat s 4 ramana ramana@allu.com

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat l

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat d 3

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat g 2

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat g 4

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./ex17_ec db.dat l

make clean

