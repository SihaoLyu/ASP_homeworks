import sys
import math
from random import random

f = open(sys.argv[1])
raw_rcd = f.read()
f.close()
records = []
while raw_rcd.find('(') != -1:
    l_pos = raw_rcd.find('(')
    r_pos = raw_rcd.find(')')
    rcd = raw_rcd[l_pos : r_pos+1]
    records.append(rcd)
    raw_rcd = raw_rcd.replace(rcd, '', 1)

disorder_records = []
length = len(records)
for i in range(length):
    seed = random()
    pos =  int(math.floor(len(records) * seed))
    disorder_records.append(records.pop(pos))

for rcd in disorder_records: print(rcd)