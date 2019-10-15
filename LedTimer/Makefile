# $@ is target
# $^ is all prerequisites
# $? is changed (newer than target) prerequisites
# $< is first prerequisite

all: LedTimer.hex

clean:
	rm -f *.hex *.ihx *.bin *.lk *.lst *.map *.mem *.rel *.rst *.sym
	rm -f LedTimer.asm

flash: LedTimer.hex
	stcgal -P stc8 $^

reset:
	stcgal -P stc8

LedTimer.hex: LedTimer.c
	sdcc $^
	packihx LedTimer.ihx > LedTimer.hex

%.rel: %.c %.h
	sdcc -c $<
