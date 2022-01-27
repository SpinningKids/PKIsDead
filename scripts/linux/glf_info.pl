#!/usr/bin/perl

# extract some info about a .glf font
# preview the glyphs it contains
#
#       -- Aragorn/sPINNING kIDS

use strict;

my $previewer = 1; # is previewer enabled? (requires Perl/Tk)
my $dumpchars = 0; # dump to stderr the chars held by the font

if ($previewer) {
    use Tk;
}

die <<EOM unless @ARGV == 1;
Usage:
$0 fontname.glf
EOM

my ($header, $name, $nsyms);

sub r_ubyte ($$) { return (unpack "C", (substr ${$_[0]}, $_[1], 1)); };

open FH, shift or die "Cannot open";
sysread FH, $header, 128;

die "Not a GLF font\n" if (substr $header, 0, 3) ne 'GLF';

$name  = substr $header, 3, 96; $name =~ s/\000.*//s;
$nsyms = r_ubyte \$header, 3+96;
print "--Header-------\n";
print "  Font name     : \`\`", $name, "''\n";
print "  Total symbols : $nsyms\n";
print "---------------\n";

sub x_vertices($$) {
    my ($datar, $nv) = @_;
    my @verts = ();
    foreach my $i (0..($nv - 1)) {
	push @verts, [
		      (unpack 'f', (substr $$datar, $i*(4*2), 4)),
		      (unpack 'f', (substr $$datar, $i*(4*2)+4, 4))
		     ];
    }
    return @verts;
}

sub x_faces($$) {
    my ($datar, $nf) = @_;
    my @faces = ();
    foreach my $i (0..($nf - 1)) {
	push @faces, [
		      (unpack 'C', (substr $$datar, $i*3  , 1)),
		      (unpack 'C', (substr $$datar, $i*3+1, 1)),
		      (unpack 'C', (substr $$datar, $i*3+2, 1))
		     ];
    }
    return @faces;
}

print "--Symbols------\n";
my $sdata;
for my $sn (0..($nsyms-1)) {
    my ($char, $nverts, $nfaces, $nlines);
    my @verts = ();
    my @faces = ();
    sysread FH, $sdata, 4;
    $char   = r_ubyte \$sdata, 0;
    $nverts = r_ubyte \$sdata, 1;
    $nfaces = r_ubyte \$sdata, 2;
    $nlines = r_ubyte \$sdata, 3;
    printf "  Symbol %d (code: %d, char: \`\`%s'', vertices: %d, faces: %d, lines: %d)\n",
      $sn, $char, pack('C',$char), $nverts, $nfaces, $nlines;
    print STDERR pack('C',$char) if $dumpchars;
    sysread FH, $sdata, (4*$nverts*2); # vertices
    @verts = x_vertices(\$sdata, $nverts);
    sysread FH, $sdata, (3*$nfaces);   # faces
    @faces = x_faces(\$sdata, $nfaces);
    sysread FH, $sdata, (  $nlines);   # lines
    # x_lines... don't care for now

    # here comes a cheap previewer... (space for the next glyph, escape or q to quit)
    if ($previewer) {
	my $W    = 100; my $W2 = $W/2;
	my $H    = 100; my $H2 = $H/2;
	my $main = MainWindow->new();
	$main->configure(-background => 'Blue', -width => $W, -height => $H);
	$main->bind('<space>', sub { $main->destroy; } );
	$main->bind('<Escape>', sub { exit; } );
	$main->bind('<q>', sub { exit; } );
	$main->geometry("${W}x${H}-10+10");
	my $c= $main->Canvas(-width => $W,-height => $H)->pack;

	foreach my $face (@faces) {
	    my @poly = ();
	    foreach (@$face) {
		push @poly, ($verts[$_][0], $verts[$_][1]);
	    }
	    $c->createPolygon(@poly, -tags => ['charpolys']);
	}

	$c->scale('charpolys',0,0,$W2,-$H2);
	$c->xviewScroll($H2*10,'units');
	$c->yviewScroll($W2*10,'units');
	MainLoop;
    }
}
print "---------------\n";

exit;
