#!/usr/bin/env python3

import sys
import glob
import time
import math
import os.path
import subprocess


if __name__ == "__main__":
    sources = {
        "vert" : glob.glob("shaders/*.vs.glsl"),
        "frag" : glob.glob("shaders/*.fs.glsl")
    }
    start = time.time()
    for stage, paths in sources.items():
        for path in paths:
            file_name = os.path.split(path)[-1]
            blob_name = file_name[:-4] + "blob"
            cmd = ["glslangValidator.exe", "-V", path, "-S", stage, "-o", os.path.join("blobs", blob_name)]
            subprocess.run(cmd, stdout=sys.stdout, stderr=sys.stdout)
    stop = time.time()
    delta = stop - start
    adjusted = math.floor(delta * 100)/100
    print("Build time: {} second(s)".format(adjusted))
