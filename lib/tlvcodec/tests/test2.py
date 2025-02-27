# some_file.py
import sys
# caution: path[0] is reserved for script path (or '' in REPL)
sys.path.insert(1, '/path/to/application/app/folder')

import os

# Get the full path of the current file
current_file = __file__
print(__file__)

# Get the directory of the current file
current_dir = os.path.dirname(current_file)
print(current_dir)
