#!/bin/sh

set files =  (\
../../TEST.bib \
../../bibutils_3.4/bib2xml/main.bib \
../../bibutils_3.4/bib2xml/dudei.bib \
../../bibutils_3.4/bib2xml/utp.cell.new.bib \
../../bibutils_3.4/bib2xml/test.bib \
../../bibutils_3.4/bib2xml/inbook.bib \
../../bibutils_3.4/bib2xml/test1.bib \
../../bibutils_3.4/xml2bib/main_1.0.bib \
../../bibutils_3.6/bib2xml/c003.bib \
../../bibutils_3.7/bib2xml/test2.bib \
)

@ count = 1
foreach file ( $files )
	( ./bib2xml_old $file >! old_${count}.xml ) >& /dev/null
	( ./bib2xml $file >! new_${count}.xml ) >& /dev/null
	set ndiff = ` diff old_${count}.xml new_${count}.xml | wc | awk '{print $1;}'`
	if ( $ndiff == 0 ) then
		echo PASSED $count $file
	else
		echo FAILED $count $file
	endif
	@ count = $count + 1
end
