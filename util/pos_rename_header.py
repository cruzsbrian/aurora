#!/usr/bin/env python

'''
Renames a KiCad component placement (pos) .csv file headers
as appropriate for JLCPCB SMT assembly service 
'''

import sys
import fileinput

for line in fileinput.input(sys.argv[1], inplace=True):
    if fileinput.isfirstline():
        print('Designator,Val,Package,Mid X,Mid Y,Rotation,Layer')
    else:
        print(line, end='')
