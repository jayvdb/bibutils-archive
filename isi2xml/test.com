#!/bin/sh

set files =  (\
../../ieee/*.isi \
)

@ count = 1
foreach file ( $files )
	( ./isi2xml_old $file >! old_${count}.xml ) >& /dev/null
	( ./isi2xml $file >! new_${count}.xml ) >& /dev/null
	set ndiff = ` diff old_${count}.xml new_${count}.xml | wc | awk '{print $1;}'`
	if ( $ndiff == 0 ) then
		echo PASSED $file
	else
		echo FAILED $file
	endif
	@ count = $count + 1
end
