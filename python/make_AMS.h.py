#!/usr/bin/env python2.2

import os, re

file = open("USE_THESE_ROOT_CLASSES")
filenew = open("AMS.h","w")
troot_h = open("TROOT.h","w")

root_inc_dir = os.popen('$ROOTSYS/bin/root-config --incdir').read().rstrip()

file.seek(0)
before_object_class = ""
after_object_class = "public:\n"
for line in file:
     match = re.search(r'^(T\w+).h\s*$',line)
     if match:
            cl = match.group(1)
            if cl != "TObject":
                  before_object_class += "class " + cl + ";\n"
                  after_object_class += "      " + cl + "* to_" + cl \
                           + "(){ return (" + cl + "*)this;};\n"

file.seek(0)
for line in file:
      chfile = line.rstrip()
      inc_file = open(root_inc_dir + "/" + chfile)
      is_namespace_root = None
      for inc_line in inc_file:
            if chfile == "TROOT.h":
                  if re.search(r'operator\s+new',inc_line): continue
                  filenew.write(inc_line)
                  troot_h.write(inc_line)
            elif chfile == "TObject.h":
                  if re.search(r'^\s*class\s+TObject\s*\{',inc_line):
                        filenew.write(before_object_class)
                        filenew.write(inc_line)
                        filenew.write(after_object_class)
                  else:
                        filenew.write(inc_line)
            else:
                  filenew.write(inc_line)

      inc_file.close()

file.close()
filenew.close()
troot_h.close()
