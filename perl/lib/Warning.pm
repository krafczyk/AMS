#  $Id: Warning.pm,v 1.2 2001/01/22 17:32:53 choutko Exp $
package Warning;

@EXPORT = qw( error );

use strict;
use CGI::Carp qw( fatalsToBrowser );

BEGIN {
    sub carp_error {
        my $error_message = shift;
        my $q = new CGI;
        my $discard_this = $q->header( "text/html" );
        error( $q, $error_message );
    }
    CGI::Carp::set_message( \&carp_error );
}

sub error {
    my( $q, $error_message ) = @_;

    print $q->header( "text/html" ),
          $q->start_html( "Error" ),
          $q->h1( "Error" ),
          $q->p( "Sorry, the following error has occurred: " ),
          $q->p( $q->i( $error_message ) ),
          $q->end_html;
    exit;
}




