import subprocess
from pathlib import Path

from buildkit import builder, model, utils


def make_relative(func):
    def wrap(self, *args):
        nargs = []
        for a in args:
            if isinstance(a, Path):
                nargs.append(Path("/") / a.relative_to(self.root))
            else:
                nargs.append(a)

        return func(self, *nargs)

    return wrap


class Image:
    def __init__(
        self,
        name: str,
        r: model.Registry,
        target: str | model.Target,
        *,
        ubsan: bool = False,
        debug: bool = False,
    ):
        self.name = name

        self.target: model.Target
        if isinstance(target, model.Target):
            self.target = target
        else:
            t = r.lookup(target)
            assert t is not None and isinstance(t, model.Target)
            self.target = t

        self.r = r.bind(self.target)

        if ubsan:
            self.r.define("ubsan", True)
            self.target.tools["cc"].args.append("-fsanitize=undefined")
            self.target.tools["cxx"].args.append("-fsanitize=undefined")

        if debug:
            self.target.tools["cc"].args.append("-ggdb")
            self.target.tools["cxx"].args.append("-ggdb")

    @property
    def root(self) -> Path:
        p = self.r.project.buildkitDir / "images" / self.name

        if not p.exists():
            p.mkdir(parents=True)

        return p

    def build(self, name: str, dest: Path):
        if not dest.parent.exists():
            dest.parent.mkdir(parents=True)

        c = self.r.lookup(name)
        assert c is not None and isinstance(c, model.Component)
        out = builder._build([c], self.target, self.r)[0]
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

    @make_relative
    def export_limine(self, kernel: Path, modules: list[Path]):
        p = self.root / "boot"
        if not p.exists():
            p.mkdir()

        with (p / "limine.conf").open("w") as f:
            f.writelines(
                [
                    "timeout: 0\n\n",
                    f"/{self.name}\n",
                    "    protocol: limine\n",
                    f"    kernel_path: boot():{kernel.as_posix()}\n",
                ]
                + [
                    f"    module_path: boot():{module.as_posix()}\n"
                    for module in modules
                ]
            )
