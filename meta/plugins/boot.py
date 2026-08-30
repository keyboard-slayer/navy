from buildkit import cli, model

from . import image, qemu


@cli.command
def boot(
    target: str = "kernel-x86_64",
    ubsan: bool = False,
    debug: bool = False,
    grub: bool = False,
):
    """
    Boot the OS inside of QEMU

    Args:
        target: The target that need to be used to build the OS
        ubsan: Enable the UndefinedBehaviorSanitizer.
        debug: Enable debug features
        grub: Use grub as the bootloader
    """

    r = model.Registry.ensure()

    i = image.Image(
        "navy", r, ubsan=ubsan, debug=debug, prefix="grub" if grub else "limine"
    )

    i.build("kernel", i.root / "vmnavy", target)

    if target == "kernel-x86_64" and grub:
        i.build("kernel", i.root / "vmnavy32", "kernel-x86")
        i.export_grub(i.root / "vmnavy32", [i.root / "vmnavy32"])
        i.install_grub()
    else:
        i.export_limine(i.root / "vmnavy", [])
        i.install_limine()

    qemu.Qemu("x86_64", i).setReboot(False).setShutdown(False).setE9(True).run()
