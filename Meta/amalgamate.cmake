
# Amalgamation cuts down Meta module build time by N times (N>10)
# CMake based concatenation taken from:
# http://stackoverflow.com/questions/15282444/using-cmake-how-can-i-concat-files-and-install-them

message("amalgamate.cmake: output=${TARGET_FILE}, input=${INPUT_FILES}")

string(REPLACE " " ";" INPUT_FILES_LIST ${INPUT_FILES})

function(cat IN_FILE OUT_FILE)
  file(READ ${IN_FILE} CONTENTS)
  file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

function(amalgamate)
   	foreach(cppSource ${INPUT_FILES_LIST})
 			  cat(${cppSource} ${TARGET_FILE} )
	endforeach(cppSource)
endfunction()

file(REMOVE ${TARGET_FILE} )
amalgamate()