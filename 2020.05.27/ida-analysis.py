#
# ida-analysis.py
# a simple IDAPython binary analysis script
# 2020.05.24 darell tan
#
# invoke with:
#   idat64 -c -A -S"ida-analysis.py $HOME/analysis.txt" <file.bin>
#

import sys
import idc
import idautils

f = open(idc.ARGV[1], 'a') if len(idc.ARGV) > 1 else sys.stdout
log = f.write

# log current file path
log(idc.get_input_file_path() + '\n')

# wait for auto-analysis to complete
idc.auto_wait()

# count functions
log( 'count %d\n' % len(list(idautils.Functions())) )

# print function name and addresses
for ea in idautils.Functions():
    if idc.get_func_flags(ea) & (idc.FUNC_LIB | idc.FUNC_THUNK): continue
    log( hex(ea) + ' ' + idc.get_func_name(ea)  + '\n' )

# if logging to a file, close it and exit IDA Pro
if f != sys.stdout:
    f.close()
    idc.qexit(0)
