import os
import sys
import shutil
import subprocess
import argparse

sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.append(os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))))

from proj import ROOT_DIR

BUILD_DIR = ROOT_DIR.parent / "build"
# make relative to the project root
BUILD_DIR = BUILD_DIR.relative_to(ROOT_DIR.parent)

commands = {
    "build": f"cmake --build {BUILD_DIR}",
    "clean": "rm -rf build",
    "run": f"{BUILD_DIR}/main",
    "rebuild": "cmake -S . -B build && cmake --build build",
    "init": f"cmake -S . -B {BUILD_DIR}",
    "bar": f"cmake --build {BUILD_DIR} && .\\{BUILD_DIR}\\bin\\main.exe"
}


def main():
    parser = argparse.ArgumentParser(description="Build and run the project")
    parser.add_argument("command", choices=commands.keys(), help="Command to run")
    args = parser.parse_args()
    subprocess.run(commands[args.command], shell=True)


if __name__ == "__main__":
    main()





