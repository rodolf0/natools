#!/usr/bin/env bash

function test_partition3 {
  z=$(( 1 + $RANDOM * 49 / 32767 ))
  kth=$(( $RANDOM * $z / 32767 ))
  unset data
  for ((a=0; a<$z; a++)); do
    data=(${data[*]} $(( $RANDOM * 50 / 32767 )))
  done
  pivot=${data[$kth]}

  ./test_sort $kth ${data[*]} > $$.$$

  part=($(cat $$.$$ | sed '1!d'))
  eq_idx=$(cat $$.$$ | sed '2!d' | sed 's/^b: \(.*\),.*$/\1/')
  gt_idx=$(cat $$.$$ | sed '2!d' | sed 's/^.*, c: \(.*\)$/\1/')
  rm -f $$.$$

  for i in $(seq 0 $((${#data[*]}-1))); do
    if [ $i -lt $eq_idx -a ${part[$i]} -ge $pivot ] ||
       [ $i -ge $eq_idx -a $i -lt $gt_idx -a ${part[$i]} -ne $pivot ] ||
       [ $i -ge $gt_idx -a ${part[$i]} -le $pivot ]; then
      echo ${part[*]}
      echo e: $eq_idx g: $gt_idx
      echo "Error: pivot=$pivot, idx=$i, val=${part[$i]}"
      return 1
    fi
  done
  return 0
}

for ((q=0; q<5000; q++)); do
  echo -ne "\rExecuting test $q. "
  test_partition3 || break;
done
