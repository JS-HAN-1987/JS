BITMAP_CONVERTER=./bitcvt.exe
PNGS=$(shell /bin/find *.png)
INCS=$(PNGS:.png=.png.inc)

all: $(INCS)
	#echo $(OUTS)
	#echo $(INCS)

%.png.inc: %.png
	#echo $<
	$(BITMAP_CONVERTER) -0 $<
	mv $@ ..


