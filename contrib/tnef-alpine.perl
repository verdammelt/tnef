#!/usr/bin/perl -w --
#
# decode a winmail.dat attachment and sent it's contents back to
# the user. this uses the tnef package from Mark Simpson.
# (just a hack by Urs Schürer - sitb - <urs@sitb.de>)
#
# usage: winmail.dat <tnef-file>
#
# just add:  application/ms-tnef; winmail.pl %s
# to your .mailcap

use strict;
use warnings;
use Email::MIME::Creator;
use IO::All;
use Email::Send;


# ========================================================
# configure some constants here
# --------------------------------------------------------
use vars qw($SMTP $DOMAIN $subject $body);
$SMTP = "willow";              # enter your smtp host here (host[:port])
$DOMAIN = "sitb.de";           # your internal email domainname
$subject = "Deine MS-TNEF Attachments";
$body = "Hier sind Deine Attachments";
# ========================================================

my $user = getpwuid($<);                                  # our user's name

my $winmail = $ARGV[0];                                   # the winmail.dat file
exit unless (stat($winmail));                             # no file -- nothing to do

my $tmpdir = `mktemp -dt tnefXXXXXXXXXX`;
chomp $tmpdir;
my @attachments = `tnef -t \"$winmail\"`;                 # get a list of files included in the MS-TNEF

my $ret = system("tnef -f \"$winmail\" -C $tmpdir");      # extract everything into our tmpdir
exit if ($ret); # tnef failed

my @parts;                                                # now compose a new mime email
my $part = Email::MIME->create(                           # ... with a little text body
                               attributes => {
                                              content_type => "text/plain",
                                              charset      => "iso-8859-1",
                                             },
                               body => $body,
                              );
push (@parts, $part);

foreach my $attachment (@attachments)                     # .. and loop over all attachments
{
  chomp $attachment;
  my $file = "$tmpdir/$attachment";
  next unless (stat($file));                              # skip stuff that isn't there
  my $ftype = `file -i \"$file\"`;                        # .. determine its mime type
  chomp $ftype;
  $ftype =~ s#^.+:\s+(.+)#$1#;

  $part = Email::MIME->create(                            # .. and add it as base64 attachment
                              attributes => {             # (no idea whether this is right all the time)
                                             filename     => $attachment,
                                             content_type => $ftype,
                                             encoding     => "base64",
                                             name         => $attachment,
                                             disposition  => "attachment"
                                            },
                              body => io( $file )->all,
                             );
  push (@parts, $part);
  unlink ($file);                                         # now we can remove the file
}

my $email = Email::MIME->create(                          # .. and compose the mail itself
                                header => [
                                           From => $user . '@' . $DOMAIN,
                                           To   => $user . '@' . $DOMAIN,
                                           Subject => $subject
                                          ],
                                parts => \@parts
                               );


#print $email->as_string . "\n";

my $mailer = Email::Send->new({mailer => 'SMTP'});        # .. which we sent via smtp
$mailer->mailer_args([Host => $SMTP, ssl => 0]);
$mailer->send($email->as_string);

# clean up
rmdir ($tmpdir);
