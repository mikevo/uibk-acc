#!/bin/bash

for f in `ls ./bench/` ; do
  echo -e "\n$f"
  total_run_time=0

  for i in {1..10} ; do
    run_time=$({ time -p ./bench/$f > /dev/null ; } 2> >(grep real) | awk '{ print $2 }')
    echo $run_time

    total_run_time=$(echo "$total_run_time $run_time" | awk '{print $1 + $2}')
  done

  echo "------"
  echo $total_run_time
  echo $total_run_time | awk '{print $1 / 10}'
done
