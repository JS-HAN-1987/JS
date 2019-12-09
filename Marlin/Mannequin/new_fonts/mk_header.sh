#!/bin/sh
FONTS_HEADER=fonts.h
rm -f $FONTS_HEADER

# number
for i in `find 0.number/*.inc`; do
		base=`basename $i`
		echo $base
		name=`echo $base | cut -d . -f 1`
		echo 'static const uint8_t font_'$name'[] PROGMEM = {' >> $FONTS_HEADER
		echo '	#include "0.number/'$name'.png.inc"' >> $FONTS_HEADER
		echo '};' >> $FONTS_HEADER
done

# upper letter
for i in `find 1.upper/*.inc`; do
		base=`basename $i`
		echo $base
		name=`echo $base | cut -d . -f 1`
		echo 'static const uint8_t font_upper_'$name'[] PROGMEM = {' >> $FONTS_HEADER
		echo '	#include "1.upper/'$name'.png.inc"' >> $FONTS_HEADER
		echo '};' >> $FONTS_HEADER
done

# lower letter
for i in `find 2.lower/*.inc`; do
		base=`basename $i`
		echo $base
		name=`echo $base | cut -d . -f 1`
		echo 'static const uint8_t font_lower_'$name'[] PROGMEM = {' >> $FONTS_HEADER
		echo '	#include "2.lower/'$name'.png.inc"' >> $FONTS_HEADER
		echo '};' >> $FONTS_HEADER
done

# special
for i in `find 3.special/*.inc`; do
		base=`basename $i`
		echo $base
		name=`echo $base | cut -d . -f 1`
		echo 'static const uint8_t '$name'[] PROGMEM = {' >> $FONTS_HEADER
		echo '	#include "3.special/'$name'.png.inc"' >> $FONTS_HEADER
		echo '};' >> $FONTS_HEADER
done

# big_numbers
for i in `find 4.big/*.inc`; do
		base=`basename $i`
		echo $base
		name=`echo $base | cut -d . -f 1`
		echo 'static const uint8_t '$name'[] PROGMEM = {' >> $FONTS_HEADER
		echo '	#include "4.big/'$name'.png.inc"' >> $FONTS_HEADER
		echo '};' >> $FONTS_HEADER
done

echo ""
echo "Press ENTER key!"
read


