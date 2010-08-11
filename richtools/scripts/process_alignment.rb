mx,my,mz=0,0,0
ma,mb,mg=0,0,0


Dir.glob("amoeba_output_*").each do |file|
  number=file.split("_")[2].to_i
  next if number==1

  handle=File.open(file)
  l=handle.gets.chomp.split(" ").collect{|v| v.to_f}
  x,y,z=l
  
  l=handle.gets.split(" ").collect{|v| v.to_f}
  a,b,g=l

  mx=0.9*mx+0.1*x
  my=0.9*my+0.1*y
  mz=0.9*mz+0.1*z
  ma=0.9*ma+0.1*a
  mb=0.9*mb+0.1*b
  mg=0.9*mg+0.1*g

  handle.gets
  value=handle.gets.to_f
  handle.close
  puts "#{number} #{value} #{x} #{y} #{z} #{a} #{b} #{g}"
end
