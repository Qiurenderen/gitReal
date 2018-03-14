#!/bin/bash
g++ -std=c++14 $1 -g -o a.out -lboost_system -pthread
