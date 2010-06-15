cat $1 | tr '\n' ' ' | sed 's/ //g' | sed 's/.*:=//g' | sed 's/\[//g' | sed 's/\]//g' | sed 's/;//g'
