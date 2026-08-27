import os
import platform
import subprocess
from functools import partial
from typing import Any

from buildkit import utils

from . import image


class Qemu:
    def __init__(self, arch: str, img: image.Image):
        self.arch = arch
        self.img = img
        self.settings = {
            "memory": "2G",
            "reboot": True,
            "shutdown": True,
            "debug": False,
            "cores": 4,
            "display": "none",
            "e9": False,
            "serial": False,
        }

        match self.arch:
            case "x86_64":
                self.bios = utils.wget(
                    "https://retrage.github.io/edk2-nightly/bin/RELEASEX64_OVMF.fd"
                )
            case _:
                raise RuntimeError(f"{self.arch} is not a known architecture")

    def __kvm_available(self) -> bool:
        return os.path.exists("/dev/kvm") and os.access("/dev/kvm", os.R_OK)

    def run(self):
        arg = [
            f"qemu-system-{self.arch}",
            "-m",
            str(self.settings["memory"]),
            "-smp",
            str(self.settings["cores"]),
            "-display",
            self.settings["display"],
            "-bios",
            str(self.bios),
            "-drive",
            f"format=raw,file=fat:rw:{self.img.root},media=disk",
        ]

        if self.__kvm_available() and platform.machine() == self.arch:
            arg += ["-enable-kvm", "-cpu", "host"]
        else:
            arg += ["-cpu", "max,+la57,+pdpe1gb"]

        if not self.settings["reboot"]:
            arg.append("-no-reboot")

        if not self.settings["shutdown"]:
            arg.append("-no-shutdown")

        if self.settings["e9"]:
            arg += ["-debugcon", "mon:stdio"]

        if self.settings["debug"]:
            arg += ["-d", "int,guest_errors,cpu_reset"]

        if self.settings["serial"]:
            arg += ["-serial", "mon:stdio"]

        subprocess.run(arg, check=True)

    def __setSetting(self, name: str, value: Any) -> "Qemu":
        self.settings[name] = value
        return self

    def __getattribute__(self, name: str, /) -> Any:
        if name.startswith("set") and name != "settings":
            key = name.removeprefix("set").lower()
            if key in self.settings:
                return partial(self.__setSetting, key)

        return super().__getattribute__(name)
