import angr
import claripy

proj = angr.Project("./target")

key = claripy.BVS("key", 8 * 4)

stdin = angr.SimFileStream(
    name="stdin",
    content=key,
    has_end=False,
)
state = proj.factory.full_init_state(
    args=[proj.filename],
    stdin=stdin,
)

for i in range(4):
    byte = key.get_byte(i)
    state.solver.add(byte >= 0x20)
    state.solver.add(byte <= 0x7e)

simgr = proj.factory.simulation_manager(state)

simgr.explore(find=lambda state: b"success" in state.posix.dumps(1), avoid=lambda state: b"fail" in state.posix.dumps(1))

if simgr.found:
    found = simgr.found[0]
    solution = found.solver.eval(key, cast_to=bytes)
    print("FOUND KEY:", solution)
else:
    print("No solution found")