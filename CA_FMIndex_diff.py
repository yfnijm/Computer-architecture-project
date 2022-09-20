import re
import sys

f1=open("golden.txt", 'r')
f2=open("pim_result.txt", 'r')

golden = []
pim = []

for line in f1:
    for word in line.split():
        golden.append(word)

for line in f2:
    for word in line.split():
        pim.append(word)

for i in range(len(golden)):
    if (golden[i] != pim[i]):
        print("wrong!!")
        sys.exit(1)

print("compare with golden correct !!!!!!")