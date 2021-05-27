#!/usr/bin/perl

print <<HEAD
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

/* This source code is generated by optiontable.pl - DO NOT EDIT BY HAND */

#include "curl_setup.h"
#include "easyoptions.h"

/* all easy setopt options listed in alphabetical order */
struct curl_easyoption Curl_easyopts[] = {
HEAD
    ;

my $lastnum=0;

while(<STDIN>) {
    if(/^ *CURLOPT\(([^,]*), ([^,]*), (\d+)\)/) {
        my($opt, $type, $num)=($1,$2,$3);
        my $name;
        my $ext = $type;

        if($opt =~ /OBSOLETE/) {
            # skip obsolete options
            next;
        }

        if($opt =~ /^CURLOPT_(.*)/) {
            $name=$1;
        }
        $ext =~ s/CURLOPTTYPE_//;
        $ext =~ s/CBPOINT/CBPTR/;
        $ext =~ s/POINT\z//;
        $type = "CURLOT_$ext";

        $opt{$name} = $opt;
        $type{$name} = $type;
        push @names, $name;
        if($num < $lastnum) {
            print STDERR "ERROR: $opt has bad number\n";
            exit 2;
        }
        else {
            $lastnum = $num;
        }
    }

    # alias for an older option
    # old = new
    if(/^#define (CURLOPT_[^ ]*) *(CURLOPT_\S*)/) {
        my ($o, $n)=($1, $2);
        # skip obsolete ones
        if($n !~ /OBSOLETE/) {
            $o =~ s/^CURLOPT_//;
            $n =~ s/^CURLOPT_//;
            $alias{$o} = $n;
            push @names, $o,
        }
    }
}


for my $name (sort @names) {
    my $oname = $name;
    my $a = $alias{$name};
    my $flag = "0";
    if($a) {
        $name = $alias{$name};
        $flag = "CURLOT_FLAG_ALIAS";
    }
    $o = sprintf("  {\"%s\", %s, %s, %s},\n",
                 $oname, $opt{$name}, $type{$name}, $flag);
    if(length($o) < 80) {
        print $o;
    }
    else {
        printf("  {\"%s\", %s,\n   %s, %s},\n",
                 $oname, $opt{$name}, $type{$name}, $flag);
    }
}

print <<FOOT
  {NULL, CURLOPT_LASTENTRY, 0, 0} /* end of table */
};

#ifdef DEBUGBUILD
/*
 * Curl_easyopts_check() is a debug-only function that returns non-zero
 * if this source file is not in sync with the options listed in curl/curl.h
 */
int Curl_easyopts_check(void)
{
  return (CURLOPT_LASTENTRY != ($lastnum + 1));
}
#endif
FOOT
    ;
