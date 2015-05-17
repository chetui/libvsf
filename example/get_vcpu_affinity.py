#!/usr/bin/env python

import os, sys, subprocess

try:
    p = subprocess.Popen("virsh vcpuinfo %s | grep -E 'CPU Affinity:' | cut -d ':' -f 2" % (sys.argv[1]),
            stdout = subprocess.PIPE,
            stderr = subprocess.PIPE,
            shell = True
        )
except:
    sys.stdout.write("[]")

output = "".join(p.stdout.readlines()).split("\n")
line_num = 0
content = ""
for line in output:
    affinity_flag = line.strip()
    if affinity_flag == "":
        continue
    affinity_num = ""
    cnt = 0
    for f in affinity_flag:
        if f == 'y':
            affinity_num = "%s,%d" % (affinity_num, cnt)
        cnt = cnt + 1

    content = "%s,[%s]" % ( content, affinity_num[1:])
    line_num = line_num + 1
sys.stdout.write("[" + content[1:] + "]")
