#!/bin/bash

mkdir -p root
mkdir -p log

# job submit
for i in $(seq 0 3); do
  tag="300_$i"
  echo "Submitting job for $tag ..."
  bsub -q a -J gem_$i -o log/log_$tag.txt "./gem $tag"
done

echo "All jobs submitted to queue 'a'."

# job monitor
while true; do
  njobs=$(bjobs -J 'gem_*' 2>/dev/null | grep -c 'PEND\|RUN')
  if [ "$njobs" -eq 0 ]; then
    echo "All jobs finished!"
    break
  fi
  echo "  -> $njobs jobs still running or pending... (checking again in 5 min)"
  sleep 300
done

# merge
echo "Merging root files into gain_300.root..."
hadd -f gain_300.root root/300_*.root

echo "All done! Output: gain_300.root"