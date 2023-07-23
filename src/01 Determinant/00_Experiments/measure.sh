#!/bin/sh
JSON_URL=settings.json
DATA_GENERATOR=data_generator
EXACT_PROCESSOR=exact_determinant.py

JSON_DATA=$(cat $JSON_URL)
PROCESSOR=$(echo $JSON_DATA | jq -r ".processor")
DETERMINANT_RECORD=$(echo $JSON_DATA | jq -r ".determinant_record")
CLOCK_RECORD=$(echo $JSON_DATA | jq -r ".clock_record")
DATA_FILE=$(echo $JSON_DATA | jq -r ".data_file")
echo -n "" > $DETERMINANT_RECORD
echo -n "" > $CLOCK_RECORD
echo -n "" > $DATA_FILE

for i in {3..12} # $(seq 50 50 500) $(seq 600 100 1000) # N
do
  for j in {0..9} # seed
  do
    ./data_generator $DATA_FILE $i $j
    CPP=$($PROCESSOR $DATA_FILE)
    read -r DETERMINANT CLOCK <<< "$(echo -e "$CPP" | tr '\n' '\t')"
    echo "$i $j $CLOCK" >> $CLOCK_RECORD
    PYTHON=`python3 $EXACT_PROCESSOR`
    echo $i $j $DETERMINANT $PYTHON >> $DETERMINANT_RECORD
  done
  echo "${i}"
done