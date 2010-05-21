
Howdy!

This is the source code to Power-Post 2000 Build 25b.

I planned to design a completely new version of Power-Post, but
may (at least for now) simply integrate posting capability into
Power-Grab 2002.  In the meantime I have decided to release the
source code.  

Please note the following:

(1) *PLEASE* do not release new versions of Power-Post without
    making it EXPLICITLY CLEAR that it is a modified version.
    For example, call it "Kinko's Power-Post" or something, so
    people will not ask me for support for something I did not
    write.
    
(2) If you do release a new version based on this code, simply
    give me (Chris Morse) credit in the Help/About.
    
(3) Power-Post 2000 is built with Visual C++ 6.0, uses MFC, and
    is multi-threaded.  This will not compile with other 
    compilers without making some modifications.
    
(4) At the moment, I have not written any documentation on the
    source code.  If I get enough questions, I'll write a little
    FAQ that explains the basic structure of the code.
    
(5) Power-Post 2000 is FREEWARE.  Any modified versions must
    be FREEWARE too.  The original spirit of usenet is not dead!

(6) Power-Post was one of my first internet applications that I
    wrote, and the source code isn't nearly as neat and orderly
    as I would like it to be.
    
NOTES:

  The "Hist" directory contains WAV files and is not really a
  "History" folder.  The WAV resources in the project refer to
  the WAV files in the Hist directory, so it's not safe to delete.

  Power-Post creates a background "worker" thread that receives
  and processes windows (WM_*) messages that are defined by
  the program.  This is basically how posting work gets done.
  
I think that's about it for now.

Cheers!
// CHRIS

