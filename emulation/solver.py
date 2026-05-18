from unicorn import *
from unicorn.x86_const import *

from capstone import *
from capstone.x86 import *

ADDRESS = 0x1000000
STACK    = 0x2000000
STACK_SIZE = 0x10000

CODE = bytes.fromhex(
    "f30f1efa554889e54883ec1848897de80" \
    "f3148c1e2204809d0488945f8c745f005" \
    "150000eb218b45f0c1e00589c28b45f00" \
    "1c2488b45e80fb6000fb6c001d08945f0" \
    "488345e801488b45e80fb60084c075d44" \
    "88b45f84889c7e87bffffff85c0740d8b" \
    "45f035efbeadde8945f4eb0b8b45f0351" \
    "04152218945f48b45f048c1e0204889c2" \
    "8b45f44809d0c9c3"
)

CODE_INTEGRITY_CHECK = bytes.fromhex(
    "f30f1efa554889e548897df80f3148c1e" \
    "2204809d0482b45f8483d881300000f97" \
    "c00fb6c05dc3"
)

md = Cs(CS_ARCH_X86, CS_MODE_64)
md.detail = True


def hook_code(mu, address, size, _):
    code = mu.mem_read(address, size)

    if mu.mem_read(address-0x2, 2) == b"\x0f\x31":
        mu.reg_write(UC_X86_REG_EAX, 0)
        mu.reg_write(UC_X86_REG_EDX, 0)

    for insn in md.disasm(code, address):
        print(f"0x{insn.address:x}: {insn.mnemonic} {insn.op_str}")

mu = Uc(UC_ARCH_X86, UC_MODE_64)
mu.mem_map(ADDRESS-0x1000, 2 * 1024 * 1024)
mu.mem_map(STACK, STACK_SIZE)
mu.mem_write(ADDRESS, CODE)
mu.mem_write(ADDRESS-0x27, CODE_INTEGRITY_CHECK)
mu.reg_write(UC_X86_REG_RSP, STACK + STACK_SIZE)

input_str = b"hash12345test\x00"
input_addr = 0x3000000
mu.mem_map(input_addr, 0x1000)
mu.mem_write(input_addr, input_str)

mu.reg_write(UC_X86_REG_RDI, input_addr)

mu.hook_add(UC_HOOK_CODE, hook_code)

print("Starting emulation...")

mu.emu_start(ADDRESS, ADDRESS + len(CODE) - 0x1)

print(f"Done! Hash: {mu.reg_read(UC_X86_REG_RAX):x}")