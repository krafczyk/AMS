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
               Producer=>undef,
               Server=>undef,
               DiskUsage=>undef,
               },
               pixmap=>[],
               mask=>[],
               pixmapb=>[],
               maskb=>[],
               notebookw=>undef,
               notebook=>[0,0,0],
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
my $hbox= new Gtk::HBox 1, 0;
$vbox->pack_start($hbox,1,1,0);
my $bquit =   new Gtk::Widget "Gtk::Button",
  "GtkButton::label"              =>      "Quit",
  "GtkObject::signal::clicked"     =>      sub{exit();},
  "GtkWidget::visible"              =>      1;
my $bupdate =   new Gtk::Widget "Gtk::Button",
  "GtkButton::label"                =>      "Update Status",
  "GtkObject::signal::clicked"      =>      \&Update,
  "GtkWidget::visible"              =>      1;
my $bedit =   new Gtk::Widget "Gtk::Button",
  "GtkButton::label"                =>      "Edit",
  "GtkObject::signal::clicked"      =>      \&Edit,
  "GtkWidget::visible"              =>      1;
$hbox->pack_start($bquit,1,1,0);
$hbox->pack_start($bupdate,1,1,0);
$hbox->pack_start($bedit,1,1,0);

		
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

 ($book_open, $book_open_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_open_xpm);
my ($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_g_xpm);
$self->{pixmapb}[0]=$book_closed;
$self->{maskb}[0]=$book_closed_mask;
($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_y_xpm);
$self->{pixmapb}[1]=$book_closed;
$self->{maskb}[1]=$book_closed_mask;
($book_closed, $book_closed_mask) = Gtk::Gdk::Pixmap->create_from_xpm_d($notebook->window, $transparent, @book_closed_r_xpm);
$self->{pixmapb}[2]=$book_closed;
$self->{maskb}[2]=$book_closed_mask;




$self->notebook_create_pages($notebook, 0, 2);
		
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

	$new_page_tab_pixmap->set( $book_open, $book_open_mask );
	$new_page_menu_pixmap->set( $book_open, $book_open_mask );
        my $Singleton=$monitorUI::Singleton;	
	if ( defined $old_page  and defined $Singleton->{current_page}) {
		my $old_page_tab_pixmap = ($old_page->tab_label->children)[0]->widget;
		my $old_page_menu_pixmap = ($old_page->menu_label->children)[0]->widget;
                my $num=$Singleton->{notebook}[$Singleton->{current_page}];
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
	

# 
          my @titles;
	for $i ( $start .. $end ) {
            if($i==0){
                	@titles = (
	    "Title 0",
	    "Title 1",
	    "Title 2",
	    "Title 3",
	    "Title 4",
	    "Title 5",
	    "Title 6"
	);

                $buffer="Producer";
              }elsif($i==1){
	@titles = (
	    "Title 0",
	    "Title 1",
	    "Title 2",
	    "Title 3",
	    "Title 4",
	    "Title 5",
	    "Title 6"
	);

                  $buffer="Server";
              }elsif($i==2){
	@titles = (
	    "FileSystem",
	    "Total (Mbytes)",
	    "Free (Mbytes)",
	    "% Free",
	    "OK",
	);
                 $buffer="DiskUsage";
              } 	
                        $child = new Gtk::Frame( $buffer );
                        $child->border_width( 10 );
                        $box = new Gtk::VBox( 1, 0 );
                        $box->border_width( 10 );
                        my ($scr, $clist)=create_clist(@titles);
                        $self->{clist}->{$buffer}=$clist;                          
                        $box->add($scr);
                        $child->add( $box );
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
	my (@titles, @text, $clist, $box1, $box2, $button, $separator);

	@titles = @_;

	my $i;
		my $scrolled_win = new Gtk::ScrolledWindow(undef, undef);
		$scrolled_win->set_policy('never', 'never');

		$clist = new_with_titles Gtk::CList(@titles);
		$clist->set_row_height(30);

#		$clist->signal_connect('select_row', \&select_clist);
#		$clist->signal_connect('unselect_row', \&unselect_clist);      


		$clist->set_selection_mode('browse');
        for $i (0 ... 5){
		$clist->set_column_justification($i, 'center');
		$clist->set_column_min_width($i, 100);
		$clist->set_column_auto_resize($i, 1);
            }
		$clist->border_width(5);
		$scrolled_win->add($clist);
        return $scrolled_win, $clist;

    }


sub Update{
     my ($monitor, $ok)=Monitor::Update();
     if(not $ok){
         Warning();
         return;
     }
     my $clist=$monitorUI::Singleton->{clist}->{DiskUsage};
     $clist->freeze();   
     $clist->clear();
     my @output=$monitor->getdbok();     
     my $i;
     for $i (0 ... $#output){
         my @text=@{$output[$i]};
     $clist->append(@text);
     $clist->set_pixtext($i, 4, "", 5, $monitorUI::Singleton->{pixmap}[$text[4]], $monitorUI::Singleton->{mask}[$text[4]]);
         if($text[4]>$monitorUI::Singleton->{notebook}[2]){
             $monitorUI::Singleton->{notebook}[2]=$text[4];
         }
     }
     $clist->thaw();
     notebook_update_pages $monitorUI::Singleton->{notebookw}, 0, 2;
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
