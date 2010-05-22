The list of changes:

The main change is in the function void CPosterThread::OnMessage_POST(WPARAM wParam, LPARAM lParam) where the UUE code was replaced with yEnc code.

Added YENCLEN = 128 in stdafx.h to replace '45' (number of UUEncoded characters per line in different files).

45 (whole word) everywhere in the code replaced to YENCLEN

x-newsposter header line was changed; the word 'modified' added; original homepage retained since the yenc change is so small and doesn't deserve much credit.

about box was changed to point out that it is modified version.

the LastTempl profile name was changed to LastTempl_yEnc to show yEnc in the template; forcing yEnc in the subject line (PropPage_AddSubject.cpp)

removed code in the middle of CM_BuildArticleName so part numbers are always appended to the end of the subject (if you put file numbers after part numbers newsreaders might take the latter for the former).

added possibility to authenticate after every sent command

the rest of the code can be found in Power-Post-2000-Build-25b-Source.zip published by Chris Morse, see www.CosmicWolf.com.

you can use fc in dos prompt to see exact differences between original and modified files.
