from binaryninja import *
import networkx as nx

target = "div0.exe"

target_operations = [
    binaryninja.MediumLevelILOperation.MLIL_DIVS,
    binaryninja.MediumLevelILOperation.MLIL_DIVS_DP,
    binaryninja.MediumLevelILOperation.MLIL_DIVU,
    binaryninja.MediumLevelILOperation.MLIL_DIVU_DP,
    binaryninja.MediumLevelILOperation.MLIL_FDIV,
]

addr_dic = {}

def build_symbol_graph(bv, func):
    graph = nx.DiGraph()
    for block in func.medium_level_il.ssa_form:
        for instr in block:
            # process MLIL_SET_VAR_SSA and MLIL_VAR_PHI operations
            if instr.operation in [binaryninja.MediumLevelILOperation.MLIL_SET_VAR_SSA, binaryninja.MediumLevelILOperation.MLIL_VAR_PHI]:
                try:
                    for var_written in instr.vars_written:
                        vw_str = "{}#{}".format(var_written.var, int(var_written.version))
                        addr_dic[vw_str] = instr.address
                        for var_read in instr.vars_read:
                            vr_str = "{}#{}".format(var_read.var, int(var_read.version))
                            addr_dic[vr_str] = instr.address
                            graph.add_edge(vr_str, vw_str)
                        if instr.src.operation == MediumLevelILOperation.MLIL_CONST:
                            graph.add_edge(str(instr.src.value.value), vw_str)
                except AttributeError as e:
                    pass
    return graph

def find_all_paths_from(graph, symbol):
    roots = (v for v, d in graph.in_degree() if d == 0)
    all_paths = []
    for root in roots:
        paths = nx.all_simple_paths(graph, root, symbol)
        if paths not in all_paths:
            all_paths.extend(paths)
    return all_paths

print("Analyzing file: {}".format(target))
bv = BinaryViewType.get_view_of_file(target)
bv.add_analysis_option('linearsweep')

alert_count = 0
for func in bv.functions:
    if func.name != "sub_140001070": continue
    print("Function: {}".format(func.name))
    for block in func.medium_level_il.ssa_form:
        for instr in block:
            for op in instr.postfix_operands:
                if op in target_operations:
                    try:
                        denom = instr.vars_read[-1]
                    except IndexError:
                        continue
                    denom_str = "{}#{}".format(denom.var, denom.version)
                    sg = build_symbol_graph(bv, func)
                    paths_from = find_all_paths_from(sg, denom_str)
                    for pf in paths_from:
                        if pf[0] == '0':
                            alert_count += 1
                            print("\n[ALERT {}]: Possible divide by zero detected.".format(alert_count))
                            print("  Function:    {}".format(func.name))
                            print("  Index:       {}".format(instr.instr_index))
                            print("  Address:     {}".format(hex(instr.address).rstrip("L")))
                            print("  Operation:   {}".format(instr.operation.name))
                            print("  Instruction: {}".format(instr))
                            print("  Variable:    {}".format(denom_str))
                            print("  Chain:       {}".format(pf[::-1]))
                            for node in pf:
                                if node in addr_dic.keys():
                                    print(hex(addr_dic[node]))
                                    func.set_auto_instr_highlight(addr_dic[node],  HighlightStandardColor.RedHighlightColor)