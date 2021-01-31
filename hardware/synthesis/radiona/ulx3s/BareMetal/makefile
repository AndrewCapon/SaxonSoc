VERILOG = ../../../../netlist/Ulx3sBareMetal.v

FPGA_SIZE ?= 85

SDRAM_SIZE ?= 32

FPGA_KS ?= $(FPGA_SIZE)k

IP ?= 192.168.0.89

ifeq ($(FPGA_SIZE), 12)
	CHIP_ID=0x21111043
	FPGA_KS = 25k
endif
ifeq ($(FPGA_SIZE), 25)
	CHIP_ID=0x41111043
endif
ifeq ($(FPGA_SIZE), 45)
	CHIP_ID=0x41112043
endif
ifeq ($(FPGA_SIZE), 85)
	CHIP_ID=0x41113043
	NEXTPNR_FLAGS += "--timing-allow-fail"
endif

IDCODE ?= $(CHIP_ID)

compile : bin/toplevel.bit

prog: bin/toplevel.bit
	ujprog $<

ftpprog: bin/toplevel.bit
	ftpecp5prog ${IP} $<

generate:
	(cd ../../../../..; sbt "runMain saxon.board.radiona.ulx3s.Ulx3sBareMetal")

bin/toplevel.json: ${VERILOG}
	mkdir -p bin
	rm -f Ulx3sBareMetal.v*.bin
	cp ../../../../netlist/Ulx3sBareMetal.v*.bin . | true
	yosys \
		-p "synth_ecp5 -json $@" \
		$<

bin/toplevel.config: bin/toplevel.json
	nextpnr-ecp5 \
		--json $< \
		--textcfg $@ \
		--lpf BareMetal.lpf \
		--$(FPGA_KS) \
        --freq 25 \
		--package CABGA381 ${NEXTPNR_FLAGS}

bin/toplevel.bit: bin/toplevel.config
	ecppack --idcode $(IDCODE) $< $@

clean:
	$(RM) -rf bin *.bin
