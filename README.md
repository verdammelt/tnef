
                                   TNEF
                                   ====

TNEF is a program for unpacking MIME attachments of type
"application/ms-tnef". This is a Microsoft only attachment.

Due to the proliferation of Microsoft Outlook and Exchange mail servers,
more and more mail is encapsulated into this format.

The TNEF program allows one to unpack the attachments which were
encapsulated into the TNEF attachment.  Thus alleviating the need to use
Microsoft Outlook to view the attachment.

TNEF is mainly tested and used on GNU/Linux and CYGWIN systems.  It
'should' work on other UNIX and UNIX-like systems.

See the file COPYING for copyright and warranty information.

See the file INSTALL for instructions on installing TNEF.  The short form
for installation is the standard:

    autoreconf
    ./configure
    make check
    make install

'make check' will have created the tnef executable in the src
directory and run all the unit tests.  'make install' should have put
it into /usr/local/bin.  Please see the man page or tnef --help for
options.
