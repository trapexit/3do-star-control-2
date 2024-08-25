# Star Control 2: The Ur-Quan Masters

This repository contains the source code and misc resources to the
1994 3DO title Star Control 2.

* https://en.wikipedia.org/wiki/Star_Control_II

> Star Control II: The Ur-Quan Masters is a 1992 video game, the
> sequel to Star Control. It was developed by Toys for Bob (Fred Ford
> and Paul Reiche III) and originally published by Accolade in 1992
> for MS-DOS. The game includes exoplanet-abundant star systems,
> hyperspace travel, extraterrestrial life, and interstellar
> diplomacy. There are 25 alien races with which communication is
> possible.
> 
> Released to critical acclaim, Star Control II is widely viewed today
> as one of the greatest PC games ever made. It has appeared on
> lists of the greatest video games of all time.
> 
> The game was ported to 3DO by Crystal Dynamics in 1994 with an
> enhanced multimedia presentation. The source code of the 3DO port
> was licensed under GPL-2.0-or-later in 2002, the game content
> under CC-BY-NC-SA-2.5. The 3DO source code was the basis of the open
> source game The Ur-Quan Masters.

## Rerelease Of Source Code

In 2002 a modified version of the 3DO source code to Start Control 2
was released as ["The Ur-Quan
Masters"](https://en.wikipedia.org/wiki/The_Ur-Quan_Masters). The code
released was already substantially modified from the original 3DO code
in an effort to port to libSDL and as a result was missing a lot of
3DO specific code.

In May 2024, [trapexit](https://github.com/trapexit) reached out to
some of the original email addresses for those who worked on the
project early on inquiring if the original 3DO source was still around
somewhere. While most of the email addresses were defunct Alex
Volkov's address was still active. It turned out that a poorly
converted version (Mac -> Windows)  of the code was in Alex's backups
and after approval from the IP holders released the archive to
trapexit.


## Contents

The archive appears to be reasonably complete in terms of the source
but has little or no media assets. Due to the improper conversation
from classic Mac formats a number of binary files may be corrupted. It
is not known if this is the retail version of the code or something
before (or perhaps after) the 3DO release.

The files have been modified and renamed where appropriate to address
improper conversions.

It is possible that an uncorrupted variant of the archive is stored in
old tape backups of Felix Lazarev (felix) but he has yet to look
through them all.


## Future Plans

Like with other 3DO source code releases there is a intent to port the
title to build against the [3do-devkit
project](https://github.com/trapexit/3do-devkit) in order to provide a
convenient base for new development.
