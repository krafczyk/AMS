/*! \example ams_inspect.rb
 *
 *    - SCRIPT ams_inspect, to plot interactively variables from AMS root files:
 *                - ams_inspect.rb [files]
 *
 *    - EXAMPLES: 
 *          - ams_inspect.rb 
 *          - ams_inspect.rb /f2users/choutko/g3v1g3.root 
 *          - ams_inspect.rb /f2users/choutko/g3v1g3*
 *
 *    - IMPORTANT: 
 *          - Please set the $AMSDir environment variable before using the script
 *          - It uses the "Ruby" scripting language
 *          - For the time being it is only available via /f2users/alcaraz/Linux/bin/ruby
 *
 */

#!/f2users/alcaraz/Linux/bin/ruby
#   TK GUI to plot AMS variables
#   Juan Alcaraz, Jan-2004

require "tk"

if ARGV.size==0
      $myfiles = TkVariable.new("/f2users/choutko/g3v1g3.root")
else
      $myfiles = TkVariable.new(ARGV.join(" "))
end
$cutmin = TkVariable.new("-1.e10<")
$cutmax = TkVariable.new("!=0")

TkRoot.new {
  title "AMS INSPECTOR"
  background "black"
}

$showfiles = TkLabel.new {
   files_line_by_line = $myfiles.to_s.gsub(/ /,"\n")
   text "AMS INSPECTOR\n\n =====CURRENT FILE(S)=====\n" +\
        "#{files_line_by_line}\n" + \
        "\n ======CURRENT CUTS=======\n" + \
   "#{$cutmin.to_s} variable_in_plot \t\t\t variable_in_plot #{$cutmax.to_s}\n"
   background "black"
   foreground "white"
   pack
}

framefile = TkFrame.new {
      background "black"
      pack(:pady=>10)
}

frame0 = TkFrame.new {
      background "black"
      pack(:pady=>10)
}

mainframe = TkFrame.new {
      background "darkblue"
      borderwidth 20
}

def amsplot(v,fil)
   varfinal = "$AMSDir/ut/amsplot '" + $cutmin.to_s + v + $cutmax.to_s + "' " + fil.to_s + " 2> /dev/null"
   fork{`#{varfinal}`}
end

def topinfo_update
   files_line_by_line = $myfiles.to_s.gsub(/ /,"\n")
   $showfiles.configure(
   "text"=>"AMS INSPECTOR\n\n =====CURRENT FILE(S)=====\n" +\
        "#{files_line_by_line}\n" + \
        "\n ======CURRENT CUTS=======\n" + \
   "#{$cutmin.to_s} variable_in_plot \t\t\t variable_in_plot #{$cutmax.to_s}\n"
   )
end

labelfile = TkLabel.new(framefile) {
  text "Use file(s): "
  background "black"
  foreground "white"
}
labelfile.pack(:side=>:left,:padx=>5)

selfile = TkEntry.new(framefile,:textvariable=>$myfiles) {
      background "white"
      foreground "black"
      relief "sunken"
      width 70
      bind('Return',proc{topinfo_update})
}
selfile.pack(:side=>:left, :fill=>:both, :expand=>true)

label0 = TkLabel.new(frame0) {
  text "Apply the following cuts:"
  background "black"
  foreground "white"
}
label0.pack(:side=>:left,:padx=>5)

cut0 = TkEntry.new(frame0,:textvariable=>$cutmin) {
      justify "right"
      background "white"
      foreground "black"
      relief "sunken"
      width 10
      bind('Return',proc{topinfo_update})
}
cut0.pack(:side=>:left,:fill=>:both, :expand=>true)

label1 = TkLabel.new(frame0) {
  text "variable_in_plot"
  background "black"
  foreground "white"
}
label1.pack(:side=>:left,:padx=>5)

cut1 = TkEntry.new(frame0,:textvariable=>$cutmax) {
      background "white"
      foreground "black"
      relief "sunken"
      width 10
      bind('Return',proc{topinfo_update})
}
cut1.pack(:side=>:left,:fill=>:both, :expand=>true)

TkButton.new { 
      background "red"
      foreground "black"
      text "EXIT"
      command 'exit'
      pack(:pady=>15)
}

root_h = ENV["AMSDir"] + "/include/root.h"
if FileTest.exists?(root_h)
      root_h_file = File.new(root_h,"r")
else
      puts "Header file \"#{root_h}\" does not exist; EXIT"
      exit
end

other = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
trd = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
tof = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
tr = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
anti = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
rich = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}
ecal = TkFrame.new(mainframe) {
      background "darkblue"
      pack(:pady=>10)
}

list_of_classes = []
current_class = nil
public_flag = false
if_comments = false
barstar_comments = false
variables = []
for line in root_h_file
      if line =~ /^#if/
            if_comments = true
            next
      elsif line =~ /^#endif/
            if_comments = false
            next
      end
      if if_comments
            next
      end
      if line =~ /^\s*\/\*/
            barstar_comments = true
            next
      elsif line =~ /^\s*\*\//
            barstar_comments = false
            next
      end
      if barstar_comments
            next
      end
      if line =~ /^(.+)\/\/\//
            line = $1
      elsif line =~ /^\/\/\//
            next
      end
      if line =~ /^class\s+(.+)R\s*[:\{]/
            if current_class
              if current_class[0..3]=="Ecal"
                mycol1 = "lightgrey"
                myframe = ecal
              elsif current_class[0..3]=="Rich"
                mycol1 = "cyan"
                myframe = rich
              elsif current_class[0..3]=="Anti"
                mycol1 = "orange"
                myframe = anti
              elsif current_class[0..2]=="Tof"
                mycol1 = "lightgreen"
                myframe = tof
              elsif current_class[0..2]=="Trd"
                mycol1 = "pink"
                myframe = trd
              elsif current_class[0..1]=="Tr"
                mycol1 = "lightblue"
                myframe = tr
              else 
                mycol1 = "white"
                myframe = other
              end
              TkMenubutton.new(myframe,:relief=>:raised) { |menumother|
                  foreground "black"
                  background mycol1
                  text current_class
                  i = 0
                  mycol2 = "yellow"
                  menu TkMenu.new(menumother) {
                      if current_class != "Header"
                        var = "# of " + current_class + "'s"
                        var0 = "n" + current_class 
                        add(:command,:background=>mycol2,:label=>var,:command=>proc{topinfo_update; amsplot(var0,$myfiles)})
                      end
                      variables.each { |var|
                        var2 = current_class + "." + var 
                        if var[-2..-1]=="()"
                              lab = var[0..-3]
                        else
                              lab = var
                        end
                        add(:command,:background=>mycol2,:label=>lab,:command=>proc{topinfo_update; amsplot(var2,$myfiles)})
                      }
                  }
                  pack(:side=>:left,:padx=>10)
              }
            end

            variables.clear
            current_class = $1
            if current_class=="AMSEvent"
                  current_class=nil
                  next
            end
            list_of_classes << current_class

      end
      if not current_class
            next
      end

      if line =~ /^\s*#{current_class}R\(/
            next
      end
      if line =~ /^\s*public:/ 
            public_flag = true
            next
      elsif line =~ /^\s*private:/ 
            public_flag = false
            next
      elsif line =~ /^\s*protected:/ 
            public_flag = false
            next
      end

      if !public_flag
            next
      end
      if line =~ /^\s*ClassDef/
            next
      elsif line =~ /^\s*friend/
            next
      end

      if line =~ /^(\s*int\s+)i(\w+)\s*\(unsigned\s+int.*\)/
             variables.unshift("N#{$2}()")
      elsif line =~ /^(\s*int\s+)(\w+);/
             variables << "#{$2}"
      elsif line =~ /^(\s*unsigned\s+int\s+)(\w+);/
             variables << "#{$2}"
      elsif line =~ /^(\s*float\s+)(\w+);/
             variables << "#{$2}"
      elsif line =~ /^(\s*int\s+)(\w+)\[(\d+)\]\s*;/
            for i in 0...$3.to_i
                  variables << "#{$2}[#{i}]"
            end
      elsif line =~ /^(\s*unsigned int\s+)(\w+)\[(\d+)\]\s*;/
            for i in 0...$3.to_i
                  variables << "#{$2}[#{i}]"
            end
      elsif line =~ /^(\s*float\s+)(\w+)\[(\d+)\]\s*;/
            for i in 0...$3.to_i
                  variables << "#{$2}[#{i}]"
            end
      elsif line =~ /^(\s*int\s+)(\w+)\[(\d+)\]\[(\d+)\]\s*;/
            for i in 0...$3.to_i
            for j in 0...$4.to_i
                  variables << "#{$2}[#{i}][#{j}]"
            end
            end
      elsif line =~ /^(\s*unsigned int\s+)(\w+)\[(\d+)\]\[(\d+)\]\s*;/
            for i in 0...$3.to_i
            for j in 0...$4.to_i
                  variables << "#{$2}[#{i}][#{j}]"
            end
            end
      elsif line =~ /^(\s*float\s+)(\w+)\[(\d+)\]\[(\d+)\]\s*;/
            for i in 0...$3.to_i
            for j in 0...$4.to_i
                  variables << "#{$2}[#{i}][#{j}]"
            end
            end
      end

end

mainframe.pack

selfile.focus

Tk.mainloop
