BITMAP_CONVERTER=../bitcvt.exe
PNGS=$(shell /bin/find *.png)
INCS=$(PNGS:.png=.png.inc)

all: $(INCS)

%.png.inc: %.png
	$(BITMAP_CONVERTER) -0 $<

