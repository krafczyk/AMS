package monitorUI;
use Error qw(:try);
use Gtk;
use Gtk::Atoms;
use Carp;
use strict;
use lib::Monitor;
@monitorUI::EXPORT =qw(new);

my ($book_open, $book_open_mask);

my @book_open_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c white",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");

my @book_open_r_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c red",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");

my @book_open_g_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c green",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");

my @book_open_y_xpm = (
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c yellow",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                ");


my @green_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c green",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"     XXXX       ",
"    XXXXXX      ",
"   XXXXXXXX     ",
"  XXXXXXXXXX    ",
" XXXXXXXXXXXX   ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
" XXXXXXXXXXXX   ",
"  XXXXXXXXXX    ",
"   XXXXXXXX     ",
"    XXXXXX      ",
"     XXXX       ",
"                ");
my @yellow_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c yellow",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"     XXXX       ",
"    XXXXXX      ",
"   XXXXXXXX     ",
"  XXXXXXXXXX    ",
" XXXXXXXXXXXX   ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
"XXXXXXXXXXXXXX  ",
" XXXXXXXXXXXX   ",
"  XXXXXXXXXX    ",
"   XXXXXXXX     ",
"    XXXXXX      ",
"     XXXX       ",
"                ");
my @red_xpm = (
"18 16 6 1",
"       c None s None",
".      c black",
"X      c red",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"     XXXXXX       ",
"    XXXXXXXX      ",
"   XXXXXXXXXX     ",
"  XXXXXXXXXXXX    ",
" XXXXXXXXXXXXXX   ",
"X##X###X###X###X  ",
"X#XXX#XX#X#X#X#X  ",
"X##XX#XX#X#X#X#X  ",
"XX#XX#XX#X#X##XX  ",
" ##XX#XX###X#XXX  ",
"  XXXXXXXXXX#XX   ",
"   XXXXXXXXXXX    ",
"    XXXXXXXXX     ",
"     XXXXXXX      ",
"                  ");

my @all_xpm=[
@green_xpm,
@yellow_xpm,
@red_xpm,
             ];

my @book_closed_g_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c green",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"       ..       ",
"     ..XX.      ",
"   ..XXXXX.     ",
" ..XXXXXXXX.    ",
".ooXXXXXXXXX.   ",
"..ooXXXXXXXXX.  ",
".X.ooXXXXXXXXX. ",
".XX.ooXXXXXX..  ",
" .XX.ooXXX..#O  ",
"  .XX.oo..##OO. ",
"   .XX..##OO..  ",
"    .X.#OO..    ",
"     ..O..      ",
"      ..        ",
"                ");


my @book_closed_r_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c red",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"       ..       ",
"     ..XX.      ",
"   ..XXXXX.     ",
" ..XXXXXXXX.    ",
".ooXXXXXXXXX.   ",
"..ooXXXXXXXXX.  ",
".X.ooXXXXXXXXX. ",
".XX.ooXXXXXX..  ",
" .XX.ooXXX..#O  ",
"  .XX.oo..##OO. ",
"   .XX..##OO..  ",
"    .X.#OO..    ",
"     ..O..      ",
"      ..        ",
"                ");


my @book_closed_y_xpm = (
"16 16 6 1",
"       c None s None",
".      c black",
"X      c yellow",
"o      c red",
"O      c #808080",
"#      c white",
"                ",
"       ..       ",
"     ..XX.      ",
"   ..XXXXX.     ",
" ..XXXXXXXX.    ",
".ooXXXXXXXXX.   ",
"..ooXXXXXXXXX.  ",
".X.ooXXXXXXXXX. ",
".XX.ooXXXXXX..  ",
" .XX.ooXXX..#O  ",
"  .XX.oo..##OO. ",
"   .XX..##OO..  ",
"    .X.#OO..    ",
"     ..O..      ",
"      ..        ",
"                ");


sub Edit{
}

sub new{
my %fields=(
            window=>undef,
            clist=>{
#               Producer_ActiveHosts=>undef,
#               Producer_ActiveClients=>undef,
#               Producer_Runs=>undef,
#               Server=>undef,
               DiskUsage=>undef,
               },
               pixmap=>[],
               mask=>[],
               pixmapb=>[],
               maskb=>[],
               notebookw=>undef,
               statusbar=>undef,
               notebook=>[0,0,0,0],
               current_page=>undef,
            );
    my $type=shift;
    my $self={
        %fields,
    };




Gtk->init;



$self->{window}  = new Gtk::Widget    "Gtk::Window",
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "AMS Production Monitor",
                "GtkWindow::allow_grow"           =>      1,
                "GtkWindow::allow_shrink"         =>      1,
                "GtkWindow::auto_shrink"         =>      0,
                "GtkContainer::border_width"      =>      10;


my $vbox= new_child {$self->{window}} "Gtk::VBox" ;

#my $menubar = new Gtk::MenuBar;
#$vbox->pack_start($menubar, 0, 1, 0);
#my $menu = new Gtk::Menu;    
#my $menuitem=new Gtk::MenuItem("Update Status");
#$menuitem->signal_connect('activate',\&Update);
#$menu->append($menuitem);
#$menuitem=new Gtk::MenuItem("Quit");
#$menuitem->signal_connect('activate',sub{exit();});
#$menu->append($menuitem);
#
#$menuitem = new Gtk::MenuItem("File");
#$menuitem->set_submenu($menu);
#$menubar->append($menuitem);
#$menuitem = new Gtk::MenuItem("Help");
#$menubar->append($menuitem);
#
#$menuitem = new Gtk::MenuItem("About");
#$menubar->append($menuitem);

my @item_factory_entries = (
	["/_File",	undef,	0,	"<Branch>"],
	["/File/tearoff1",	undef,	0,	"<Tearoff>"],
	["/File/_Update Status",	"<alt>U",	1],
	["/File/sep1",	undef,	0,	"<Separator>"],
	{
		'path' => "/File/_Quit", 
		'accelerator' => "<alt>Q",	
		'action' => 5,
		'type' => '<Item>'
	},
      ["/_Help",	undef,	0,	"<Item>"],
      ["/_About",	undef,	0,	"<Item>"],
                        );

		my $accel_group = new Gtk::AccelGroup;
		my $item_factory = new Gtk::ItemFactory('Gtk::MenuBar', "<main>", $accel_group);
		
		$accel_group->attach($self->{window});
		foreach (@item_factory_entries) {
			$item_factory->create_item($_, \&item_factory_cb);
		}
$vbox->pack_start($item_factory->get_widget('<main>'), 0, 0, 0);
my $hbox= new Gtk::HBox 1, 0;
$vbox->pack_start($hbox,1,1,0);
#my $bquit =   new Gtk::Widget "Gtk::Button",
#  "GtkButton::label"              =>      "Quit",
#  "GtkObject::signal::clicked"     =>      sub{exit();},
#  "GtkWidget::visible"              =>      1;
#my $bupdate =   new Gtk::Widget "Gtk::Button",
#  "GtkButton::label"                =>      "Update Status",
#  "GtkObject::signal::clicked"      =>      \&Update,
#  "GtkWidget::visible"              =>      1;
#my $bedit =   new Gtk::Widget "Gtk::Button",
#  "GtkButton::label"                =>      "Edit",
#  "GtkObject::signal::clicked"      =>      \&Edit,
#  "GtkWidget::visible"              =>      1;
#$hbox->pack_start($bquit,1,1,0);
#$hbox->pack_start($bupdate,1,1,0);
#$hbox->pack_start($bedit,1,1,0);

		
my $separator = new Gtk::HSeparator();
$vbox->pack_start( $separator, 0, 1, 10 );


my $notebook = new Gtk::Notebook;
$self->{notebookw}=$notebook;
$notebook->signal_connect( 'switch_page', \&notebook_page_switch );
$notebook->set_tab_pos(-top);
$vbox->pack_start($notebook, 1, 1, 0);
$notebook->border_width(10);
$notebook->realize;
my $transparent;

		
    my $mybless=bless $self,$type;
    if(ref($monitorUI::Singleton)){
        croak "Only Single monitorUI Allowed\n";
    }
    $monitorUI::Singleton=$mybless;

 ($book_open, $book_open_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_open_g_xpm);
$self->{pixmapbo}[0]=$book_open;
$self->{maskbo}[0]=$book_open_mask;
 ($book_open, $book_open_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_open_y_xpm);
$self->{pixmapbo}[1]=$book_open;
$self->{maskbo}[1]=$book_open_mask;
 ($book_open, $book_open_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_open_r_xpm);
$self->{pixmapbo}[2]=$book_open;
$self->{maskbo}[2]=$book_open_mask;
my ($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_g_xpm);
$self->{pixmapb}[0]=$book_closed;
$self->{maskb}[0]=$book_closed_mask;
($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_y_xpm);
$self->{pixmapb}[1]=$book_closed;
$self->{maskb}[1]=$book_closed_mask;
($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_r_xpm);
$self->{pixmapb}[2]=$book_closed;
$self->{maskb}[2]=$book_closed_mask;


$self->notebook_create_pages($notebook, 0, 3);

my $statusbar = new Gtk::Statusbar;
$vbox->pack_end($statusbar, 1, 1, 0);
$self->{statusbar}=$statusbar;
if (not $Monitor::Singleton->{ok}){
    $statusbar->push(1," Unable to Connect Servers");
}else{
    $statusbar->push(1," Connected to Servers");
}		
Gtk->timeout_add(60000,\&Update);
return $mybless;
}


sub notebook_page_switch {

        my($widget, $new_page, $page_num)  = @_;

#	print "switch page $page_num\n";

	my $old_page = $widget->cur_page();
	if ( defined $old_page ) {
		if ($new_page == $old_page) {
                        
			return;
		}
	}
       
	my $new_page_tab_pixmap = ($new_page->tab_label->children)[0]->widget;
	my $new_page_menu_pixmap = ($new_page->menu_label->children)[0]->widget;

#	$new_page_tab_pixmap->set( $book_open, $book_open_mask );
#	$new_page_menu_pixmap->set( $book_open, $book_open_mask );
        my $Singleton=$monitorUI::Singleton;	
                my $num=$Singleton->{notebook}[$page_num];
		$new_page_tab_pixmap->set( $Singleton->{pixmapbo}[$num],  $Singleton->{maskbo}[$num]  );
		$new_page_menu_pixmap->set( $Singleton->{pixmapbo}[$num],  $Singleton->{maskbo}[$num]  );
	if ( defined $old_page  and defined $Singleton->{current_page}) {
		my $old_page_tab_pixmap = ($old_page->tab_label->children)[0]->widget;
		my $old_page_menu_pixmap = ($old_page->menu_label->children)[0]->widget;
                $num=$Singleton->{notebook}[$Singleton->{current_page}];
		$old_page_tab_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
		$old_page_menu_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
                                         
            }
        $Singleton->{current_page}=$page_num;
    }


sub notebook_update_pages {
        my( $widget, $start, $end)=@_;
	for my $i ( $start .. $end ) {
            $widget->set_page($i);
            my $new_page=  $widget->cur_page();
	my $new_page_tab_pixmap = ($new_page->tab_label->children)[0]->widget;
	my $new_page_menu_pixmap = ($new_page->menu_label->children)[0]->widget;
        my $Singleton=$monitorUI::Singleton;	
                my $num=$Singleton->{notebook}[$Singleton->{current_page}];
		$new_page_tab_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
		$new_page_menu_pixmap->set( $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num]  );
            
        }
            $widget->set_page(0);
}
sub notebook_create_pages {
	  my( $self, $notebook, $start, $end ) = @_;
	my(
		$child,
		$label,
		$entry,
		$box,
		$hbox,
		$label_box,
		$menu_box,
		$button,
		$pixwid,
		$i,
		$buffer
	);
	
	for $i ( $start .. $end ) {
            if($i==0){
                $buffer="Producer Processes";
              }elsif($i==1){
                  $buffer="Producer I/O";
              }elsif($i==2){
                  $buffer="Server";
              }elsif($i==3){
                 $buffer="DiskUsage";
              } 	

# 
            $child = new Gtk::VBox (1,0);
#            my $child2 = new Gtk::VBox (1,0);
#            $child->pack_start($child2,0,1,0);
            $child->show_all();
                my $Singleton=$monitorUI::Singleton;	
		$label_box = new Gtk::HBox 0, 0;
                my $num=$Singleton->{notebook}[$i];
		my $pixwid = new Gtk::Pixmap $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num];
		$label_box->pack_start($pixwid, 0, 1, 0);
		$pixwid->set_padding(3, 1);
		$label = new Gtk::Label $buffer;
		$label_box->pack_start($label, 0, 1, 0);
		show_all $label_box;
                $menu_box = new Gtk::HBox( 0, 0 );
		$pixwid = new Gtk::Pixmap $Singleton->{pixmapb}[$num],  $Singleton->{maskb}[$num];
                $menu_box ->pack_start( $pixwid, 0, 1, 0 );
                $pixwid->set_padding( 3, 1);
                $label = new Gtk::Label( $buffer) ;
                $menu_box->pack_start( $label, 0, 1, 0 );
                $menu_box->show_all();
		
		
		$notebook->append_page_menu($child, $label_box, $menu_box);

          my (@titles,$policy_x,$policy_y);
            if($i==0){
                	@titles = (
	    "HostName",
	    "Running Pr",
	    "Allowed Pr",
	    "Failed Pr",
	    " Ntuples",
            "EventTags ",
	    " Events ",
	    "  % of Total",
	    " Errors  ",
	    " CPU/Event (sec)",
	    " Efficiency",
	    "Status ",
	);

        $policy_x="never";
        $policy_y="never";
                $buffer="Producer_ActiveHosts";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,20,@titles);

                        $#titles=-1;
                	@titles = (
	    "ID",
	    "HostName",
	    "Process ID",
	    "Start Time",
	    "LastUpdate Time",
            "Run",
            "Ntuple",
            "Status",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Producer_ActiveClients";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,18,@titles);
              }elsif($i==1){

                        $#titles=-1;
                	@titles = (
	    "Run",
	    "Time",
	    "First Event",
	    "Last Event",
	    "Priority ",
	    "History ",
	    "Status ",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Producer_Runs";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,16,@titles);

                        $#titles=-1;
                	@titles = (
	    "Run",
	    "Time",
	    "First Event",
	    "Last Event",
	    "Ntuple",
	    "Status ",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Producer_Ntuples";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,16,@titles);

              }elsif($i==2){

                        $#titles=-1;
                	@titles = (
	    "ID",
	    "HostName",
	    "Process ID",
	    "Start Time",
	    "LastUpdate Time",
            "Status",
	);

        $policy_x='automatic';
        $policy_y='automatic';
                $buffer="Server_ActiveClients";
                create_frame ($child,$self,$buffer,$policy_x,$policy_y,20,@titles);
              }elsif($i==3){
	@titles = (
	    "FileSystem",
	    "Total (Mbytes)",
	    "Free (Mbytes)",
	    "% Free",
	    "OK",
	);
        $policy_x='automatic';
        $policy_y='automatic';
                 $buffer="DiskUsage";
              create_frame ($child,$self,$buffer,$policy_x,$policy_y,20,@titles);
              } 	




        }
        my $Singleton=$monitorUI::Singleton;	
            
     my($pixmap, $mask) =create_from_xpm_d Gtk::Gdk::Pixmap( $Singleton->{window}->window, undef,  @green_xpm );
          $self->{pixmap}[0]=$pixmap;
          $self->{mask}[0]=$mask;
     ($pixmap, $mask) =create_from_xpm_d Gtk::Gdk::Pixmap( $Singleton->{window}->window, undef,  @yellow_xpm );
          $self->{pixmap}[1]=$pixmap;
          $self->{mask}[1]=$mask;
     ($pixmap, $mask) =create_from_xpm_d Gtk::Gdk::Pixmap( $Singleton->{window}->window, undef,  @red_xpm );
          $self->{pixmap}[2]=$pixmap;
          $self->{mask}[2]=$mask;
      }




sub create_clist {
	my (@titles, @text, $clist, $box1, $box2, $button, $separator, $policy_x,$policy_y, $size);

	( $policy_x, $policy_y,$size, @titles) = @_;

	my $i;
		my $scrolled_win = new Gtk::ScrolledWindow(undef, undef);
		$scrolled_win->set_policy($policy_x, $policy_y);

		$clist = new_with_titles Gtk::CList(@titles);
		$clist->set_row_height($size);

#		$clist->signal_connect('select_row', \&select_clist);
#		$clist->signal_connect('unselect_row', \&unselect_clist);      


		$clist->set_selection_mode('browse');
        for $i (0 ... $#titles){
		$clist->set_column_justification($i, 'center');
		$clist->set_column_min_width($i, 20);
		$clist->set_column_auto_resize($i, 1);
            }
		$clist->border_width(5);
		$scrolled_win->add($clist);
        return $scrolled_win, $clist;

    }


sub Update{
    $monitorUI::Singleton->{statusbar}->push(1," Retreiving Server Information");
    while (Gtk->events_pending()){
           Gtk->main_iteration();
       }
     my ($monitor, $ok)=Monitor::Update();
     if(not $ok){
         $monitorUI::Singleton->{statusbar}->push(1," Unable to Update Servers");
         Warning();
         return;
     }else{
       $monitorUI::Singleton->{statusbar}->push(1," Connected to Servers");
     }

{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_ActiveHosts};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getactivehosts();     
     my $i;
     $monitorUI::Singleton->{notebook}[0]=0;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[0]){
             $monitorUI::Singleton->{notebook}[0]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}
{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_ActiveClients};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getactiveclients("Producer");     
     my $i;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[0]){
             $monitorUI::Singleton->{notebook}[0]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}


{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_Runs};
     $clist->freeze();   
     $clist->clear();
     $monitorUI::Singleton->{notebook}[1]=0;
     my @output=$monitor->getruns;
     my $i;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[1]){
             $monitorUI::Singleton->{notebook}[1]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}

{
     my $clist=$monitorUI::Singleton->{clist}->{Producer_Ntuples};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getntuples;
     my $i;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
     }
     $clist->thaw();

}


{
     my $clist=$monitorUI::Singleton->{clist}->{Server_ActiveClients};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getactiveclients("Server");     
     my $i;
     $monitorUI::Singleton->{notebook}[2]=0;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
         my $pmc=$#text-1;
     $clist->set_pixtext($i, $pmc, $text[$pmc], 5, $monitorUI::Singleton->{pixmap}[$text[$pmc+1]], $monitorUI::Singleton->{mask}[$text[$pmc+1]]);
         if($text[$pmc+1]>$monitorUI::Singleton->{notebook}[2]){
             $monitorUI::Singleton->{notebook}[2]=$text[$pmc+1];
         }
     }
     $clist->thaw();

}

{
     my $clist=$monitorUI::Singleton->{clist}->{DiskUsage};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getdbok();     
     my $i;
     $monitorUI::Singleton->{notebook}[3]=0;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
     $clist->set_pixtext($i, 4, "", 5, $monitorUI::Singleton->{pixmap}[$text[4]], $monitorUI::Singleton->{mask}[$text[4]]);
         if($text[4]>$monitorUI::Singleton->{notebook}[3]){
             $monitorUI::Singleton->{notebook}[3]=$text[4];
         }
     }
     $clist->thaw();

}

     notebook_update_pages $monitorUI::Singleton->{notebookw}, 0, 2;
    warn "herehere ";
}

sub Warning(){
 my $s="\7";
 print $s;

my $window  =   new Gtk::Widget    "Gtk::Window",
                "GtkWidget::has_focus"            =>      1,
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "Unable Connect Server",
                "GtkWindow::allow_grow"           =>      0,
                "GtkWindow::modal"                =>      1,
                "GtkWindow::window_position"             =>      -center,
                "GtkWindow::allow_shrink"         =>      0,
                "GtkContainer::border_width"      =>      10;

   
		$window->signal_connect( 'destroy', \&Gtk::Widget::destroyed, \$window );
                
                $window->realize();
                my $box = new Gtk::VBox(1,0);
                $window->add($box);
		my $label_box = new Gtk::HBox 0, 0;
                my ($book_closed, $book_closed_mask) = create_from_xpm_d Gtk::Gdk::Pixmap( $window->window, undef, @red_xpm );
		my $pixwid = new Gtk::Pixmap $book_closed, $book_closed_mask;
		$label_box->pack_start($pixwid, 0, 1, 0);
		$pixwid->set_padding(3, 1);
		my $label = new Gtk::Label "Unable To connect Server";
		$label_box->pack_start($label, 0, 1, 0);
                $box->pack_start($label_box,0,1,0);


        	my $button = new Gtk::Button( 'Close' );
		$button->signal_connect( 'clicked', sub { $window->destroy } );
                $box->pack_start($button,0,1,0);
#                Gtk->grab_add($window);
                show_all $window;
}

sub item_factory_cb {
	my ($widget, $action, @data) = @_;
        if($action==5){
            exit();
        }elsif($action==1){
          Update();
        }
}

sub create_frame{
    my ($boxa,$self,$buffer,$policy_x, $policy_y, $size,@titles )= @_;
                        my $child = new Gtk::Frame( $buffer );
                        $child->border_width( 10 );
                        my $box = new Gtk::VBox( 1, 0 );
                        $box->border_width( 10 );
                        my ($scr, $clist)=create_clist($policy_x, $policy_y, $size,@titles);
                        $self->{clist}->{$buffer}=$clist;                          
                        $box->add($scr);
                        $child->add( $box );
		        $boxa->pack_start($child, 0,1,0);
}
