#!/usr/local/bin/perl -w
$FILE=">/afs/cern.ch/user/c/choutko/AMS/prod/RunTable";
    open FILE or die "ups\n";
$DIR="/Offline/RunsDir/runs";
opendir(DIR,$DIR) or die "Jopa $!"; 
@ml=readdir(DIR);

foreach $line (@ml){
    print $line."\n";
    if($line =~ /\./){
    }
    else{
if ($line =~ /71/) {
    print FILE "$line 1 0 10 $DIR/$line"."\n";
}
else {
 if ($line =~ /65/){
    print FILE "$line 10000 20000 12 $DIR/$line"."\n";
 } 
 else{
    print FILE "$line 1 0 2 $DIR/$line"."\n";
 }
}
}
}
