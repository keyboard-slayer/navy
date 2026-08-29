import subprocess
from pathlib import Path

from buildkit import builder, model, utils


def make_relative(func):
    def wrap(self, *args):
        nargs = []
        for a in args:
            if isinstance(a, Path):
                nargs.append(Path("/") / a.relative_to(self.root))
            elif isinstance(a, list) and a and isinstance(a[0], Path):
                nargs.append([Path("/") / p.relative_to(self.root) for p in a])
            else:
                nargs.append(a)

        return func(self, *nargs)

    return wrap


class Image:
    def __init__(
        self,
        name: str,
        r: model.Registry,
        *,
        ubsan: bool = False,
        debug: bool = False,
        prefix: str | None = None,
    ):
        self.name = name
        self.prefix = prefix

        self.r = r

        if ubsan:
            self.r.define("ubsan", True)

        if debug:
            self.r.define("debug", True)

    @property
    def root(self) -> Path:
        if self.prefix is not None:
            p = self.r.project.buildkitDir / "images" / self.prefix / self.name
        else:
            p = self.r.project.buildkitDir / "images" / self.name

        if not p.exists():
            p.mkdir(parents=True)

        return p

    def build(self, name: str, dest: Path, target: str | model.Target):
        if isinstance(target, str):
            t = self.r.lookup(target)
            assert isinstance(t, model.Target)
        else:
            t = target

        reg = self.r.bind(t)

        if not dest.parent.exists():
            dest.parent.mkdir(parents=True)

        c = reg.lookup(name)
        assert c is not None and isinstance(c, model.Component)
        out = builder._build([c], t, reg)[0]
        out.copy(dest)

    def install_limine(self):
        bootx64 = self.root / "efi" / "boot" / "bootx64.efi"

        if bootx64.exists():
            return

        bootx64.parent.mkdir(parents=True, exist_ok=True)

        archive = utils.wget(
            "https://github.com/Limine-Bootloader/Limine/releases/download/v12.5.2/limine-binary.tar.gz"
        )

        proc = subprocess.run(
            ["tar", "xzf", str(archive), "--to-stdout", "limine-binary/BOOTX64.EFI"],
            capture_output=True,
        )

        with open(self.root / "efi" / "boot" / "bootx64.efi", "wb") as f:
            f.write(proc.stdout)

    def install_grub(self):
        bootx64 = self.root / "efi" / "boot" / "bootx64.efi"

        if bootx64.exists():
            return

        bootx64.parent.mkdir(parents=True, exist_ok=True)

        grub_cfg = self.root / "boot" / "grub" / "grub.cfg"

        subprocess.run(
            [
                "grub-mkstandalone",
                "-O",
                "x86_64-efi",
                "-o",
                str(bootx64),
                "--modules=part_gpt part_msdos fat normal boot linux configfile multiboot2",
                f"boot/grub/grub.cfg={grub_cfg}",
            ],
            check=True,
        )

    @make_relative
    def export_limine(self, kernel: Path, modules: list[Path]):
        p = self.root / "boot"
        p.mkdir(exist_ok=True)

        with (p / "limine.conf").open("w") as f:
            f.writelines(
                [
                    "timeout: 0\n",
                    f"/{self.name}\n",
                    "    protocol: limine\n",
                    f"    kernel_path: boot():{kernel.as_posix()}\n",
                ]
                + [
                    f"    module_path: boot():{module.as_posix()}\n"
                    for module in modules
                ]
            )

    @make_relative
    def export_grub(self, kernel: Path, modules: list[Path]):
        p = self.root / "boot" / "grub"
        p.mkdir(exist_ok=True, parents=True)

        with (p / "grub.cfg").open("w") as f:
            f.writelines(
                [
                    "set timeout=0\n",
                    "set default=0\n",
                    "insmod all_video\n",
                    f'menuentry "{self.name}" {{\n',
                    "    search --no-floppy --set=root --file /vmnavy32\n",
                    f"    multiboot2 {kernel.as_posix()}\n",
                ]
                + [
                    f'    module2 {module.as_posix()} "{module.name}"\n'
                    for module in modules
                ]
                + ["   boot\n", "}"]
            )
