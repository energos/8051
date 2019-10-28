# $@ is target
# $^ is all prerequisites
# $? is changed (newer than target) prerequisites
# $< is first prerequisite

all: LedTimer.hex

clean:
	rm -f *.hex *.ihx *.bin *.lk *.lst *.map *.mem *.rel *.rst *.sym
	rm -f LedTimer.asm

flash: LedTimer.hex
#	stcgal -P stc8 $^
#	minipro -p 87c51fa -w LedTimer.hex
	stcgal -P stc89 $^

verify: LedTimer.hex
#	minipro -p 87c51fa -m LedTimer.hex
	@echo "Not available for this device"

reset:
#	stcgal -P stc8
#	@echo "Not available for this device"
	stcgal -P stc89

LedTimer.hex: LedTimer.c
	sdcc $^
	packihx LedTimer.ihx > LedTimer.hex

%.rel: %.c %.h
	sdcc -c $<
