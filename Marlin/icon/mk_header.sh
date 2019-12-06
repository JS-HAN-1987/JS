#!/bin/sh
echo "" > oc_icon.h
for i in `find *.inc`; do
		name=`echo $i | cut -d . -f 1`
		echo 'static const uint8_t '$name'[] PROGMEM = {' >> oc_icon.h
		echo '	#include "'$name'.png.inc"' >> oc_icon.h
		echo '};' >> oc_icon.h
done

