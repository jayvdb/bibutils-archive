#!/bin/sh

set files =  (\
../../bibutils_3.5/ris2xml/main.ris \
../../bibutils_3.5/ris2xml/main1.ris \
../../bibutils_3.5/ris2xml/test.ris \
../../bibutils_doc/test2.ris \
../../bibutils_doc/testrm.ris \
../../ieee/01007072.ris \
../../ieee/compendex.ris \
../../ieee/proquest-exporttofile.ris \
../../ieee/proquest-saveas.ris \
../../ieee/sciencedirect.ris \
../../ris_end/htmltags.ris \
../../ris_end/01007072.ris \
)

@ count = 1
foreach file ( $files )
	( ./ris2xml_old $file >! old_${count}.xml ) >& /dev/null
	( ./ris2xml $file >! new_${count}.xml ) >& /dev/null
	set ndiff = ` diff old_${count}.xml new_${count}.xml | wc | awk '{print $1;}'`
	if ( $ndiff == 0 ) then
		echo PASSED $count $file
	else
		echo FAILED $count $file
	endif
	@ count = $count + 1
end
