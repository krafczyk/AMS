package monitorUI;
use Error qw(:try);
use Gtk;
use Carp;
use strict;
@monitorUI::EXPORT =qw(new);
sub Edit{
}

sub new{
my %fields=(
            window=>undef,
            );

    my $type=shift;
    my $self={
        %fields,
    };




Gtk->init;

$self->{window}  =   new Gtk::Widget    "Gtk::Window",
                "GtkWindow::type"                 =>      -toplevel,
                "GtkWindow::title"                =>      "AMS Production Monitor",
                "GtkWindow::allow_grow"           =>      1,
                "GtkWindow::allow_shrink"         =>      1,
                "GtkContainer::border_width"      =>      10;


my $vbox= new_child {$self->{window}} "Gtk::VBox";
my $hbox= new_child $vbox "Gtk::HBox";

my $bquit =   new Gtk::Widget "Gtk::Button",
  "GtkButton::label"              =>      "Quit",
  "GtkObject::signal::clicked"     =>      sub{exit();},
  "GtkWidget::visible"              =>      1;
my $bupdate =   new Gtk::Widget "Gtk::Button",
  "GtkButton::label"                =>      "Update Status",
  "GtkObject::signal::clicked"      =>      \&Monitor::Update,
  "GtkWidget::visible"              =>      1;
my $bedit =   new Gtk::Widget "Gtk::Button",
  "GtkButton::label"                =>      "Edit",
  "GtkObject::signal::clicked"      =>      \&Edit,
  "GtkWidget::visible"              =>      1;
$hbox->pack_start($bquit,1,1,0);
$hbox->pack_start($bupdate,1,1,0);
$hbox->pack_start($bedit,1,1,0);



    return bless $self,$type;
}

