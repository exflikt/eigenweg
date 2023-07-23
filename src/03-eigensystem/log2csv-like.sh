#!/bin/sh

awk -v NO_FIELDS=${2:-4} '
 /^\[.* matrix\]$/
{
NxN = substr($1, 2)
N = substr(NxN, 0, index(NxN, "x") - 1)
for (i = 0; i < 9; i++) {
  getline mat_id
  printf("%s-%smat,%s", NxN, substr(mat_id, 3), N)
  for (j = 0; j < NO_FIELDS; j++) {
    getline field
    printf(",%s", substr(field, index(field, ":") + 2))
  }
  printf("\n")
}
getline
}' ${1:-test-jacobi.log} \
| grep -v '^\[.*\]$'
