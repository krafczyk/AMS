/*! \example ams_inspect.rb
 *
 *    - SCRIPT ams_inspect, to plot interactively variables from AMS root files:
 *                - ams_inspect.rb [files]
 *
 *    - EXAMPLES: 
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

if ARGV.size<1
      puts "SCRIPT ams_inspect.rb: "
      puts "\t\tams_inspect.rb [files]"
      puts "\t to plot interactively AMS variables from a set of root files"
      exit
else
      myfiles = ARGV.join(" ")
end

TkRoot.new {
  title "AMS INSPECTOR"
  background "darkblue"
}

TkLabel.new {
  files_line_by_line = ARGV.join("\n")
  text "AMS INSPECTOR\n\n FILES:\n#{files_line_by_line}\n"
  background "darkblue"
  foreground "white"
  pack
}

TkButton.new { |menumother|
      background "red"
      foreground "black"
      text "EXIT"
      command 'exit'
      pack
}

root_h = ENV["AMSDir"] + "/include/root.h"
if FileTest.exists?(root_h)
      root_h_file = File.new(root_h,"r")
else
      puts "Header file \"#{root_h}\" does not exist; EXIT"
      exit
end

other = TkFrame.new {
      background "darkblue"
      pack(:pady=>10)
}
trd = TkFrame.new {
      background "darkblue"
      pack(:pady=>10)
}
tof = TkFrame.new {
      background "darkblue"
      pack(:pady=>10)
}
tr = TkFrame.new {
      background "darkblue"
      pack(:pady=>10)
}
anti = TkFrame.new {
      background "darkblue"
      pack(:pady=>10)
}
rich = TkFrame.new {
      background "darkblue"
      pack(:pady=>10)
}
ecal = TkFrame.new {
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
                        var0 = "$AMSDir/ut/amsplot n" + current_class + " " + myfiles + " 2> /dev/null"
                        add(:command,:background=>mycol2,:label=>var,:command=>proc{fork{`#{var0}`}})
                      end
                      variables.each { |var|
                        if var[-2..-1]=="()"
                              lab = var[0..-3]
                              var2 = "$AMSDir/ut/amsplot '" + current_class + "." + var + "' " + myfiles + " 2> /dev/null"
                        else
                              lab = var
                              var2 = "$AMSDir/ut/amsplot " + current_class + "." + var + " " + myfiles + " 2> /dev/null"
                        end
                        add(:command,:background=>mycol2,:label=>lab,:command=>proc{fork{`#{var2}`}})
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

Tk.mainloop
