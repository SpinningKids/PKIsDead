# PK is Dead

Original edition of the Spinning Kids 2002 production. This repo holds the classic
version (song `Dxn_2.xm`, CoolPrint/GLFont text, `timebase[]` scene engine).
The 2026 post-pandemic edition lives in [aNewGene](https://github.com/SpinningKids/aNewGene).

```text
_______________________________________________  

                   pk is dead
                 final  release

              rio pan dixan wiss 
                spinning  kids

       ranked 4th at mekka symposium
        2002  fallingbostel germany 
________________________________________________

 the following bytes can be intended as info file
________________________________________________

 yes,spinning kids are back after about 2 years 
 of  comfortable sleeping and jerking , and the 
 best party possible to  release our handful of
 bytes is, of course, mekka symposium!
 furthermore , this is our first prod featuring
 our friend wiss , who joined our crew to bring
 some fresh canadian flavour.
 
 let's point out the credits:
 code  : rio, pan, wiss
 design: pan, rio, dixan
 music : dixan

 and then some dues:
 firelight multimedia for fmod music system
 m. oberhumer & l. molnar for UPX packer 
 liz for her patience
 philip k. dick for his nightmares
 
 now, get some notes to contact us:
 web : www.spinningkids.org
 mail: nickofthedude@spinningkids.org
________________________________________________

 I guess this is all you expected from this txt
________________________________________________

 Final words
 We would like to thank the people from 3state
 for their congrats mail just after the party =)
________________________________________________

 Love,
  PanRioDixanWiss
________________________________________________

________________________________________________

 AreYouStillAlive /PK ? final words by pan
________________________________________________

  Working on the design, I was inspired by the 
  book "Ubik" and the short novel "Misadjustment"
  both from Philip K. Dick (1928-1982).
```

_nfo reproduced verbatim from pouët.net: <https://www.pouet.net/prod_nfo.php?which=5610>_

## Build

Windows (MSVC + Ninja, from a Visual Studio developer environment):

```
cmake --preset x64-release      # or x64-debug / x86-debug / x86-release
cmake --build out/build/x64-release
```

MinGW: plain `cmake -B out/build/mingw` + `cmake --build out/build/mingw` (MINGW support: `6eddfc1` + minifmod v2.1.0-2+; see [PKIsDead#2](https://github.com/SpinningKids/PKIsDead/issues/2) for the v2.2.2 bump).

Linux: legacy SDL1 + glf text path; the `if (!WIN32)` include bug was fixed (`if (NOT WIN32)`), full Linux-build verification is tracked in [PKIsDead#3](https://github.com/SpinningKids/PKIsDead/issues/3).
