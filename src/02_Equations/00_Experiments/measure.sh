#!/bin/sh
JSON_URL=settings.json
DATA_GENERATOR=data_generator
EXACT_PROCESSOR=exact_equation.py
MAX=1000

JSON_DATA=$(cat $JSON_URL)
PROCESSOR=$(echo $JSON_DATA | jq -r ".processor")
EQUATION_RECORD=$(echo $JSON_DATA | jq -r ".equation_record")
CLOCK_RECORD=$(echo $JSON_DATA | jq -r ".clock_record")
DATA_FILE=$(echo $JSON_DATA | jq -r ".data_file")
echo -n "" > $EQUATION_RECORD
echo -n "" > $CLOCK_RECORD
echo -n "" > $DATA_FILE

for i in $(seq 50 50 500) $(seq 600 100 1000) # N
do
  for j in {0..9} # seed
  do
    ./data_generator $DATA_FILE $i $j
    CPP=$($PROCESSOR $DATA_FILE 1)
    read -r -d';' EQUATION <<< "$(echo -e "$CPP" | tr '\n' ';')"
    read -r CLOCK <<< "$(echo -e "$CPP" | tail -n 1)"
    echo "$i $j $CLOCK" >> $CLOCK_RECORD
    PYTHON=`python3 $EXACT_PROCESSOR`
    (
      echo -n $i $j $EQUATION
      for k in `seq ${i} $(($MAX-1))`
      do 
        echo -n "0 "
      done
      echo -n $PYTHON
      for k in `seq ${i} $(($MAX-1))`
      do 
        echo -n "0 "
      done
      echo ""
    ) >> $EQUATION_RECORD
  done
  echo "${i}"
done