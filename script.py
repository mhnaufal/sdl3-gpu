from typing import List

import os
import argparse
import subprocess
import sys

ANDROID_NDK: str = f"C:\\Users\\{os.getlogin()}\\AppData\\Local\\Android\\Sdk\\ndk-bundle"

def main():
    parser = argparse.ArgumentParser(description="Bubuk Project Helper Commands")
    parser.add_argument("--platform", default="windows", help="Platform option (windows & android)")
    parser.add_argument("--build-dir", default="build", help="Build directory")
    parser.add_argument("--generator", help="CMake generator (e.g., Ninja, Visual Studio)")
    parser.add_argument("--build-type", default="Release", help="Build type (Debug & Release)")
    parser.add_argument("--clean", action="store_true", help="Clean build directory before building")
    args = parser.parse_args()

    build_with_cmake(
        platform=args.platform,
        build_dir=args.build_dir,
        generator=args.generator,
        build_type=args.build_type,
        clean=args.clean
    )

def build_with_cmake(platform="windows", build_dir="build", generator="Visual Studio 17 2022", build_type="Release", clean=False):
    print(ANDROID_NDK)

    # Create build directory if it doesn't exist
    if not os.path.exists(build_dir):
        os.makedirs(build_dir)
    elif clean:
        print(f"🐳 Cleaning build directory: {build_dir}")
        for filename in os.listdir(build_dir):
            file_path = os.path.join(build_dir, filename)
            try:
                if os.path.isfile(file_path) or os.path.islink(file_path):
                    os.unlink(file_path)
                elif os.path.isdir(file_path):
                    import shutil
                    shutil.rmtree(file_path)
            except Exception as e:
                print(f"🐦 Failed to delete {file_path}. Reason: {e}")

    # Change to build directory
    os.chdir(build_dir)

    # CMake configure command
    cmake_cmd = ["cmake", "..", f"-DCMAKE_BUILD_TYPE={build_type}"]
    if generator:
        cmake_cmd.extend(["-G", generator])
    
    if platform == "windows":
        cmake_cmd.extend(["-A", "x64"])
    elif platform == "android":
        cmake_cmd.extend([
            f"-DCMAKE_TOOLCHAIN_FILE=${ANDROID_NDK}/build/cmake/android.toolchain.cmake"
            f"-DANDROID_ABI=arm64-v8a"
            f"-DANDROID_PLATFORM=android-24"
        ])

    # Run CMake configure
    print("🐳 Running CMake configure... ")
    print(f"COMMAND: {' '.join(cmake_cmd)}")
    result = subprocess.run(cmake_cmd)
    if result.returncode != 0:
        print("🐦 CMake configure failed!")
        sys.exit(result.returncode)

    # Build command
    build_cmd = ["cmake", "--build", ".", "--parallel", "10"]
    if platform == "windows":
        build_cmd.extend(["--config", build_type])
    elif platform == "android":
        pass
    
    # Run build
    print("\n🐳 Running build... ")
    print(f"COMMAND: {' '.join(build_cmd)}")
    result = subprocess.run(build_cmd)
    if result.returncode != 0:
        print("🐦 Build failed!")
        sys.exit(result.returncode)

    print("\n🦖 Build completed successfully!")

def is_vscode():
    return "VSCODE_CWD" in os.environ

def get_vscode_build_type():
    return os.getenv("buildType", "Debug")

if __name__ == "__main__":
    main()
