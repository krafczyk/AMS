#!/usr/bin/env ruby
$VERBOSE = nil

file = File.new("USE_THESE_ROOT_CLASSES")
filenew = File.new("AMS.h","w")
troot_h = File.new("TROOT.h","w")

root_inc_dir = `$ROOTSYS/bin/root-config --incdir`.chomp

file.seek(0)
before_object_class = ""
after_object_class = "public:\n"
for line in file
     if line =~ /^(T\w+).h\s*$/
            if $1 != "TObject"
                  before_object_class << "class #{$1};\n"
                  after_object_class << "      #{$1}* to_#{$1}(){ return (#{$1}*)this;};\n"
            end
     end 
end

file.seek(0)
for line in file
      chfile = line.chomp
      inc_file = File.new("#{root_inc_dir}/#{chfile}")
      is_namespace_root = false
      for inc_line in inc_file
            if chfile == "TROOT.h"
                  next if inc_line =~ /operator new/
                  filenew.puts inc_line
                  troot_h.puts inc_line
            elsif chfile == "TObject.h"
                  if inc_line =~ /^\s*class\s+TObject\s*\{/
                        filenew.puts before_object_class
                        filenew.puts inc_line
                        filenew.puts after_object_class
                  else
                        filenew.puts inc_line
                  end 
#            elsif chfile == "Rtypes.h"
#                  if inc_line =~ /^namespace\s+ROOT\s*\{\s*$/
#                        is_namespace_root = true
#                        puts inc_line
#                        next
#                  elsif is_namespace_root == true
#                        if inc_line =~ /^\}[;\s]*$/
#                              is_namespace_root = false
#                              puts inc_line
#                              next
#                        elsif inc_line =~ /^\}[;\s]*\/\//
#                              is_namespace_root = false
#                              puts inc_line
#                              next
#                        end
#                  else
#                        filenew.puts inc_line
#                  end
            else
                  filenew.puts inc_line
            end
      end
      inc_file.close
end

file.close
filenew.close
troot_h.close
