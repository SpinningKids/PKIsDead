#!/usr/bin/perl

# .ttf to .glf coverter
# uses font3d with POV3 output (see the example font3d.def)
# NOTE: font3d doesn't handle composite glyphs (glyphs that
#       derive from other ones, e.g.: left angle bracket is
#       built by mirroring the right one) so you have to find
#       a program able to convert those special glyphs to
#       ``standard'' ones. I use High Logic's ``Font Creator
#       Program'' (http://www.high-logic.com/). If you not do
#       so, some glyphs are skipped.
#
#       -- Aragorn/sPINNING kIDS

use strict;

my $previewer = 0; # is previewer enabled? (requires Perl/Tk)

# transformations (tweak these to adjust your chars size/position)
my $OFS_Y = -0.28; # offset on the y axis
my $SCL_W = 2.0;   # width scale factor
my $SCL_H = 2.0;   # height scale factor

if ($previewer) {
    use Tk;
}

die <<EOM unless @ARGV == 2;
Usage:
$0 font3d.inc fontname.glf
EOM

my %glf =
  (
   name   => 'ttf_to_glf.pl by Aragorn/SK',
   nsyms  => 0,
   header => 'GLF' . ("\000" x (128-3)),
   sdata  => ''
  );

sub glf_add_glyph($$$) {
    my ($code, $vertsr, $facesr) = @_;
    my ($sdatar, $nverts, $nfaces, $nlines);
    $sdatar = \$glf{sdata};
    $nverts = scalar keys %$vertsr;
    $nfaces = @$facesr;
    $nlines = 0;

    $$sdatar .= pack 'C', $code;
    $$sdatar .= pack 'C', $nverts;
    $$sdatar .= pack 'C', $nfaces;
    $$sdatar .= pack 'C', $nlines;

    my @i2v;
    $#i2v = $nverts-1;
    foreach (keys %$vertsr) {
	my ($x,$y) = split ',', $_;
	$x *= $SCL_W;
	$y += $OFS_Y;
	$y *= $SCL_H;
	$i2v[$vertsr->{$_}] = "$x,$y";
    }
    for my $i (0..($nverts-1)) {
	my ($x,$y) = split ',', $i2v[$i];
	$$sdatar .= pack 'f', $x;
	$$sdatar .= pack 'f', $y;
    }
    for my $i (0..($nfaces-1)) {
	$$sdatar .= pack 'C', $vertsr->{$facesr->[$i]->[0]};
	$$sdatar .= pack 'C', $vertsr->{$facesr->[$i]->[1]};
	$$sdatar .= pack 'C', $vertsr->{$facesr->[$i]->[2]};
    }
    $glf{nsyms}++;
}

# the chars we wish to convert
my $chars='!"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~';


my (%verts, @mesh, $fc, $vc, $charcode);
for my $charidx (0..((length $chars) - 1)) {
    $charcode = (unpack 'C', (substr $chars, $charidx, 1));
    system './font3d code=' . $charcode;
    open FH, $ARGV[0] or die "Cannot open";
    %verts = @mesh = ();
    $fc = $vc = 0;
    while (<FH>) {
	chomp;
	next unless s/^\s*triangle\{\<//;
	s/\>\}$//;
	foreach (split '>,<') {
	    s/,[^,]*?$//;
	    $verts{$_} = $vc++ unless (exists $verts{$_});
	    push @{$mesh[$fc]}, $_;
	}
	$fc++;
    }
    close FH;
    printf "faces/vertices %d/%d\n", $fc, $vc;
    if ($fc == 0  or $vc == 0) {
	print STDERR "!!!WARNING skipping \`\`" . (substr $chars, $charidx, 1) . "''\n";
	next;
    }
    if ($fc > 255  or $vc > 255) {
	print STDERR "!!!WARNING skipping \`\`" . (substr $chars, $charidx, 1) . "'' (more than 255 vertices/faces)\n";
	next;
    }

    glf_add_glyph($charcode,\%verts,\@mesh);

    if ($previewer) {
	my $W    = 100; my $W2 = $W/2;
	my $H    = 100;	my $H2 = $H/2;
	my $main = MainWindow->new();
	$main->configure(-background => 'Blue', -width => $W, -height => $H);
	$main->bind('<space>', sub { $main->destroy; } );
	$main->bind('<Escape>', sub { exit; } );
	$main->bind('<q>', sub { exit; } );
	$main->geometry("${W}x${H}-10+10");
	my $c= $main->Canvas(-width => $W,-height => $H)->pack;

	my @poly;
	foreach (@mesh) {
	    @poly = ();
	    foreach (@$_) {
		my @xy;
		push @xy, (split ',', $_);
		$xy[0] *= $SCL_W;
		$xy[1] += $OFS_Y;
		$xy[1] *= $SCL_H;
		print STDERR "!!!WARNING $xy[0],$xy[1] (adjust your scale/offset parameters)\n"
		  if $xy[0] > 1 or $xy[0] < -1 or
		     $xy[1] > 1 or $xy[1] < -1;
		push @poly, @xy;
	    }
	    $c->createPolygon(@poly, -tags => ['charpolys']);
	}

	$c->scale('charpolys',0,0,$W2,-$H2);
	$c->xviewScroll($H2*10,'units');
	$c->yviewScroll($W2*10,'units');
	MainLoop;
    }
}

# build GLF header
substr $glf{header}, 3, length($glf{name}), $glf{name};
substr $glf{header}, 3+96, 1, (pack 'C', $glf{nsyms});

open FH, ">$ARGV[1]" or die "Cannot write output file\n";
syswrite FH, $glf{header}, 128;
syswrite FH, $glf{sdata}, length($glf{sdata});
close FH;

exit;
