//  $Id: monitor.cgi,v 1.4 2001/01/22 17:32:53 choutko Exp $
#!/usr/bin/perl -w
use Gtk;
use strict;


use lib::Monitor;
use lib::monitorHTML;



# activate poa and get self ior

my $monitor=new Monitor();


#activate CGI

my $html=new monitorHTML;


 my $ok=$monitor->Connect();

#get ior from server and connect to



 if($ok){
   $html->Update();
 }
 else{
  $html->Warning();
 }


