from buildkit import cli, model

from . import image, qemu


@cli.command
def boot(target: str = "kernel-x86_64", ubsan: bool = False, debug: bool = False):
    """
    Boot the OS inside of QEMU

    Args:
        target: The target that need to be used to build the OS
        ubsan: Enable the UndefinedBehaviorSanitizer.
        debug: Enable debug features
    """

    r = model.Registry.ensure()

    i = image.Image("navy", r, target, ubsan=ubsan, debug=debug)
    i.build("kernel", i.root / "vmnavy")
    i.export_limine(i.root / "vmnavy", [])
    i.install_limine()

    qemu.Qemu("x86_64", i).setReboot(False).setShutdown(False).setE9(True).run()
