#!/usr/bin/perl

# I'm too lazy to look for a bin2obj-like thing, hence:
die <<EOM unless @ARGV == 2 or @ARGV == 1;
Usage:

$0 infile.any [outfile.h]
EOM

sub BYTES { 70 / 5 }

open INF,   $ARGV[0] or die "can't open infile\n";
open OUTF, '>' . ($ARGV[1] || (($ARGV[0] !~ /\.h/i ? ($ARGV[0] =~ s/\..*?$//, shift)[1] : $ARGV[0]) || 'outfile') . '.h')  or die "can't open outfile\n";

binmode INF;

print OUTF "static unsigned char sumsymbol[] = {";
while ($n = read(INF, $string, BYTES)) {
        die "error while reading infile\n" if $n == undef;
        print OUTF ($notfirst++ ? ',' : ''), "\n\t", join(',', map { sprintf "0x%02X", $_ } unpack("C*", $string));
}
print OUTF "\n};\n";
