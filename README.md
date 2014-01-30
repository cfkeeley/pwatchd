version: pwatch-0.4 (beta release). 
license: BSD 
Platform: OpenBSD 3.3 x86

contact: Chris Keeley - crizza@sdf.lonestar.org
public key - pgp.mit.edu (search string 'crizza')

*******************************************************************************************************************************
pwatch is *not* effective with a 'block no-route' rule in the ruleset; 
naturally, 'pf' gets there first and blocks the incoming request.

NOTE: This program may not work with later releases of OpenBSD, the ioctl's have already changed somewhat in 3.5.
However, from what I gather from the 3.5 man page, the ioctl's I have used in pwatch remain the same!

Due to lack of money and bandwidth im not always able to keep up with -current. 
If there is a change in -current that effects pwatch, and you patch it, then please mail it to me here: crizza@sdf.lonestar.org
Thanks.

NOTE: pwatch has _only_ been tested within the confines of a lan.
      feel free to test it in a real world env :) and submit any bugs/patches back to me  
      so that I can update/improve the next release.

*******************************************************************************************************************************

SETUP:

$ make pwatch clean

$ make install - can be used although pwatchd does not yet reflect the install location for the config file 
                 although this can be easily changed in parseconf.h 

*******************************************************************************************************************************
--BUGS
