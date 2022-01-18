#!/bin/bash

echo "small: " >> res_log;
./small_range_rand_allocs >> res_log;
echo "large: " >> res_log;
./large_range_rand_allocs >> res_log;
echo "equal: " >> res_log;
./equal_size_allocs >> res_log;

echo >> res_log;
