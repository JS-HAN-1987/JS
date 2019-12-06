#!/bin/sh
echo "" > oc_fonts.h
for i in `find *.inc`; do
		name=`echo $i | cut -d . -f 1`
		echo 'static const uint8_t '$name'[] PROGMEM = {' >> oc_fonts.h
		echo '	#include "'$name'.png.inc"' >> oc_fonts.h
		echo '};' >> oc_fonts.h
done

