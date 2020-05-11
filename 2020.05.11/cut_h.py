import sys
import binaryninja

# 4 years ago we had to specify the loader format and sleep while analysis
# finished
target = "easy"
bv = binaryninja.BinaryViewType.get_view_of_file(target, update_analysis=True)

# start is our entry point and has two calls, the second call is main
# grabbing start isn't too different than before
#start = bv.get_function_at(bv.entry_point)
start = bv.entry_function
start_blocks = list(start.high_level_il) # start only has one block
start_calls = [x for x in start_blocks[0] if x.operation == binaryninja.HighLevelILOperation.HLIL_CALL]
call_main = start_calls[1] # second call is main

# Main has a single call to our handler function
main = bv.get_function_at(call_main.dest.constant)
main_blocks = list(main.high_level_il) # main only has one block
main_calls = [x for x in main_blocks[0] if x.operation == binaryninja.HighLevelILOperation.HLIL_CALL]
call_handler = main_calls[0] # first call is handler

# Here's where the real improvements lie
# Handler has our cookie, which is compared with memcmp in the 
# last call of the first block, but our call is folded into the if condition
handler = bv.get_function_at(call_handler.dest.constant)
handler_blocks = list(handler.high_level_il)

# grab all the HLIL_IF instructions out of the first block, which there should only be one.
if_insn = [x for x in handler_blocks[0] if x.operation == binaryninja.HighLevelILOperation.HLIL_IF]

# The call to memcmp is the left side of the condition, the right side is '0':
#   if(memcmp(buf, "cookie", 4) == 0)
call_memcmp = if_insn[0].condition.left

# Now pull the cookie's data pointer out of the call to memcmp
#   arg0 is our input buffer
#   arg1 is the cookie data pointer
#   arg2 is the size of the compare
cookie_ptr = call_memcmp.params[1].constant

# Read the first 4 bytes to get the cookie value, we could also use the 
# count of the memcmp here
cookie = bv.read(cookie_ptr, 4)
print(f"Cookie: {cookie}")