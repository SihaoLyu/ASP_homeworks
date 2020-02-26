import sys


f = open(sys.argv[1])
raw_rcd = f.read()
f.close()
records = []
users = []
while raw_rcd.find('(') != -1:
    l_pos = raw_rcd.find('(')
    r_pos = raw_rcd.find(')')
    rcd = raw_rcd[l_pos : r_pos+1]
    records.append(rcd[1:5])
    raw_rcd = raw_rcd.replace(rcd, '', 1)

for rcd in records:
    if rcd not in users: users.append(rcd)

print(len(users))