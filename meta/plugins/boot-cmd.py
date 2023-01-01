from osdk import utils, build
import shutil


def bootAarch64():
    kernel = build.buildOne("kernel-aarch64:debug", "core")

    return [
        "qemu-system-aarch64",
        "-machine", "virt",
        "-cpu", "cortex-a72",
        "-kernel", kernel,
        "-serial", "mon:stdio",
        "-m", "2G",
        "-smp", "4"
    ]

archs = {
    "aarch64": bootAarch64,
}

def bootCmd(opts: dict, args: list[str]) -> None:
    if "help" in opts or "-h" in args:
        print("Usage: osdk boot [OPTIONS...]")
        print("\nDescription:\n   Boot a component in a QEMU instance.")
        print("\nOptions:\n  --target=ARCH - Specify an architecture")
        print("  --help - Show this help message")
        print(f"\nAvailable architecture:\n{'{}'.join([f'  * {a}' for a in archs.keys()])}".format("\n"))
        return

    if "target" not in opts:
        print(f"{utils.Colors.RED}[-]{utils.Colors.RESET} No target was specified")
        exit(1)

    if opts["target"] not in archs.keys():
        print(f"{utils.Colors.RED}[-]{utils.Colors.RESET} {opts['target']} is not a valid target")
        exit(1)


    cmd = archs[opts["target"]]()
    print("\033[2J")
    utils.runCmd(*cmd)


__plugin__ = {
    "name": "boot",
    "desc": "Boot a component in a QEMU instance.",
    "func": bootCmd,
}
