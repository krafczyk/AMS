100.times do |i|
  input="amoeba_output_#{i+1}"
  output="amoeba_output_#{i+2}"
#  system "./amoeba2_single.exe '/ciemat/mdelgado/cosmics/data/magnet_on_2009/*.root' #{input} #{output}"
  system "./amoeba2_single.exe '/ciemat/mdelgado/cosmics/data/magnetoff2009/*.root' #{input} #{output}"
  system "mv amoeba_winners.root amoeba_winners_#{i+2}.root" 
  system "mv summary.root summary_#{i+2}.root" 
  system "rm amoeba_results.txt"
end
