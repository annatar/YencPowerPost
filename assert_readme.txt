Yenc Power Post A&A (bandwidth limit + multi-servers config + multi-threads + par2 + ...)

    This software in now under GPL, which means that if you redistribute
  or modify it you must distribute the source code so that others can
  improve it later on. See gpl.txt for more informations.

    This software is FREE software and should be considered BETA
  No warranties, expressed or implied, is provided. The author assumes
  no liability for damages of ANY kind which may result from the use of
  this software.

    Also, when complaining about problems, keep in mind that I did not
  force you to use it.                                      -Assert.


website: http://powerpost.cjb.net/
email: assert@powerpost.cjb.net

Version 11B (21/03/04)
======================

- Corrected NZB XML encoding issues (sorry for that...)

Version 11 (01/17/04)
======================

- Changed licence to GPL (thanks to Chris Morse)

- Added NZB file support (see www.newzbin.com)

- Corrected PAR2 creation crash with first volume number too high

- Decreased the minimum max-lines number to 500

Version 10C (09/11/03)
======================

- Corrected MD5 Hash errors on some PAR2 files (par2 informations appears as unknown in QuickPar)

- Increased the possible length of server name

- Added an option to use only last directory name in subject ($D versus full path $P)

- Added basic skin support
   To add more skin, copy one of the subdirectories and rename it. Then edit the bmp files.
   The skin.txt file must be present and contains the RGB background color and RGB text color
   of the posting queue.

Version 10A (also incorrectly named 10B) (06/25/03)
======================

- Corrected the PAR2 'corruptions' with QuickPar-generated files using the same number of lines.

- Auto-detection of par2 part size is disabled by defaults (enable it in Settings/Program)

- Rewrote CRC, hash and par2 cpu-intensive functions in assembler. Speed is now similar to QuickPar.

- Added a disclaimer

- Other bug fixes

Version 10 (06/20/03)
=====================

- Multiple posting threads (up to 4)
-> configuration in server tab

- Queue can be modified while posting

- Creation of checksum files: .sfv, .par, .par2
-> Checksum tab during the creation of the post

- CREATION OF PAR2 RECOVERY VOLUMES. This is the main modification ;)
Usage:
. Select a par2 file and click on the par2 icon (or create a .par2 checksum file when configuring post)
. Specify the number of blocs to create and whether you want equally-sized files then click OK
. Power Post will create some *.vol??.par2.temp files and add then to the queue with a pink 'Q' icon. This means they are queued for encoding
. Start encoding using the pink arrow in toolbar.
. You can see the progress using the 'Encoding Progress' icon.
. After being encoded, the .temp extention will be removed and the files will be queued for posting

- Hopefully fixed the "misnamed file" bug...

- Other minor or experimental modifications:
. Detection of .par2 packets size to send one block per usenet message (this can reduce the number of block required to repair a post in half).
. Removed all sounds (well that reduced the executable size a lot).
. Added icons in the toolbar to move tasks in queue
. Fixed a bug when using '$K' in subject with files less than 1KB
. "Content-type: message/partial" support (EXPERIMENTAL)
. Grouping multipart messages or all the files of one post using Message-IDs and References headers (EXPERIMENTAL)

- I hope I didn't forget anything...



Version 9 (04/22/03)
====================

- stop programming post only when a file is completed
- put Yenc-Power-Post-A&A-vx everywhere
- correction of bugs
- rename soft form YPP-Assert to YPP-A&A



Version 8a (03/15/03)
====================

- save queue when a file is completed



Version 8 (03/11/03)
====================

- added Add file button in the tab "File Order"



Version 7 (03/10/03)
====================

- correction of bug when file remove in the sorting dialog
- possibility to programming stop posting
- possibility to repeat the programmation every day automatically



Version 6.01 (03/08/03)
====================
ChangeLog (relative to yEnc version)

- Added bandwidth limit
- added support for multiple servers
- removed the restriction on "yEnc" in subject
- corrected the blocking when a file is queued without any part selected.
- added specific sorting of .par/.pxx files (when adding file: in tab "File Order", button "Sort .PXX"
- added delete file button (Del file)
- added my name in the title ;-)

Fell free to improve it :-)

Assert and Alain (contact: alt.binaries.cartoons.french.d).
